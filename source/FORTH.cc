#include<vector>
#include<sstream>
#include<iostream>

#include<FORTH.hh>

using namespace std;

static TOKEN
classify( const std::string& word ){
  if( word == ":" || word == ";" ){
    return TOKEN::DEFINE;
  } else if( word == "@" ){
    return TOKEN::FETCH;
  } else if( word == "!" ){
    return TOKEN::STORE;
  } else if( word == "VARIABLE" ){
    return TOKEN::DECLARE;
  } else if( word == "IF" || word == "ELSE" || word == "THEN" ){
    return TOKEN::BRANCH;
  } else if( word == "DO" || word == "LOOP" ){
    return TOKEN::LOOP;
  } else {
    return TOKEN::WORD;
  }
}

void
FORTH::handle_definition(){
  ++mTokenIndex;

  auto word = mTokens[mTokenIndex].second;
  vector<string> tokens;

  ++mTokenIndex;

  while( mTokens[mTokenIndex].second != ";" ){
    tokens.emplace_back( mTokens[mTokenIndex].second );

    ++mTokenIndex;
  }

  /* If the program looks like this:
   *
   * 1
   * X
   * 4<mTokenIndex
   *
   * Where X is a word defined as : X 2 3 ;
   * then the process to execute X looks like this:
   * 1
   * X<mTokenIndex
   * 4<insertIndex
   *
   * 1
   * X<mTokenIndex
   * 2<insertIndex
   * 4
   *
   * 1
   * X<mTokenIndex
   * 2
   * 4<insertIndex
   *
   * 1
   * X<mTokenIndex
   * 2
   * 3<insertIndex
   * 4
   *
   * 1
   * X<mTokenIndex
   * 2
   * 3
   * 4
   *
   * 1
   * 2<mTokenIndex
   * 3
   * 4
   */
  mDictionary[word] =
    [this,tokens](){
      auto insertIndex = mTokenIndex--;

      for( auto it = tokens.begin(); it != tokens.end(); ++it ){
        mTokens.emplace( ( mTokens.begin() + insertIndex ), classify( *it ), *it );
        ++insertIndex;
      }

      mTokens.erase( mTokens.begin() + mTokenIndex );
    };

  ++mTokenIndex;
}

void
FORTH::handle_declare(){
  ++mTokenIndex;

  mDictionary[mTokens[mTokenIndex].second] =
    [this](){
      mDataStack.push( mAddressCounter );
    };

  mAddressCounter++;

  ++mTokenIndex;
}

void
FORTH::handle_fetch(){
  auto index = mDataStack.top();

  mDataStack.pop();

  mDataStack.push( mMainMem[index] );

  ++mTokenIndex;
}

void
FORTH::handle_store(){
  auto index = mDataStack.top();

  mDataStack.pop();

  mMainMem[index] = mDataStack.top();

  mDataStack.pop();

  ++mTokenIndex;
}

void
FORTH::handle_word(){
  auto word = mTokens[mTokenIndex].second;
  int value;
  stringstream ss( word );

  ++mTokenIndex;

  if( ss >> value ){
    mDataStack.push( value );
  } else {
    mDictionary.at( word )();
  }
}

void
FORTH::handle_branch(){
  if( mTokens[mTokenIndex].second == "ELSE" ){
    while( mTokens[mTokenIndex].second != "THEN" ){
      ++mTokenIndex;
    }
  } else if( mTokens[mTokenIndex].second == "IF" ){
    if( !mDataStack.top() ){
      while( ( mTokens[mTokenIndex].second != "ELSE" )
          && ( mTokens[mTokenIndex].second != "THEN" ) ){
        ++mTokenIndex;
      }
    }

    mDataStack.pop();
  }

  ++mTokenIndex;
}

void
FORTH::handle_loop(){
  // call stack is used for loop control variables
  // this would look nicer if we could just store the iterator at do
  if( mTokens[mTokenIndex].second == "LOOP" ){
    auto lcv = mCallStack.top();
    mCallStack.pop();
    auto limit = mCallStack.top();
    mCallStack.pop();

    if( ++lcv < limit ){
      // go back to DO marker
      mTokenIndex = mCallStack.top() + 1;

      mCallStack.push( limit );
      mCallStack.push( lcv );
    } else {
      // we're done, pop off DO index
      mCallStack.pop();
      ++mTokenIndex;
    }
  } else if( mTokens[mTokenIndex].second == "DO" ){
    auto lcv = mDataStack.top();
    mDataStack.pop();
    auto limit = mDataStack.top();
    mDataStack.pop();

    mCallStack.push( mTokenIndex );
    mCallStack.push( limit );
    mCallStack.push( lcv );

    ++mTokenIndex;
  }
}

FORTH::FORTH()
  : mDictionary(
    {{".",
      [this](){
        cout << mDataStack.top();

        mDataStack.pop();
      }
    },
    {".\"",
      [this](){
        while( mTokens[mTokenIndex].second.back() != '\"' ){
          cout << mTokens[mTokenIndex].second;

          ++mTokenIndex;
        }

        string last = mTokens[mTokenIndex].second;

        cout << last.substr( 0, last.size() - 1 );

        ++mTokenIndex;
      }
    },
    {"CR",
      [](){
        cout << endl;
      }
    },
    {"I",
      [this](){
        mDataStack.push( mCallStack.top() );
      }
    },
    {"EMIT",
      [this](){
        cout << ( char ) mDataStack.top();

        mDataStack.pop();
      }
    },
    {"DUP",
      [this](){
        mDataStack.push( mDataStack.top() );
      }
    },
    {"SWAP",
      [this](){
        auto top = mDataStack.top();
        mDataStack.pop();

        auto two = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( top );
        mDataStack.push( two );
      }
    },
    {"OVER",
      [this](){
        auto top = mDataStack.top();
        mDataStack.pop();

        auto two = mDataStack.top();

        mDataStack.push( top );
        mDataStack.push( two );
      }
    },
    {"DROP",
      [this](){
        mDataStack.pop();
      }
    },
    {".S",
      [this](){
        auto stack_cp = mDataStack;

        while( !stack_cp.empty() ){
          cout << stack_cp.top() << " ";
          stack_cp.pop();
        }
      }
    },
    {"ROT",
      [this](){
        //! @todo implement ROT
      }
    },
    {"+",
      [this](){
        auto lhs = mDataStack.top();
        mDataStack.pop();

        auto rhs = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( lhs + rhs );
      }
    },
    {"-",
      [this](){
        auto lhs = mDataStack.top();
        mDataStack.pop();

        auto rhs = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( lhs - rhs );
      }
    },
    {"*",
      [this](){
        auto lhs = mDataStack.top();
        mDataStack.pop();

        auto rhs = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( lhs * rhs );
      }
    },
    {"/",
      [this](){
        auto lhs = mDataStack.top();
        mDataStack.pop();

        auto rhs = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( lhs / rhs );
      }
    },
    {">",
      [this](){
        auto lhs = mDataStack.top();
        mDataStack.pop();

        auto rhs = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( -( lhs > rhs ) );
      }
    },
    {"<",
      [this](){
        auto lhs = mDataStack.top();
        mDataStack.pop();

        auto rhs = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( -( lhs < rhs ) );
      }
    },
    {"=",
      [this](){
        auto lhs = mDataStack.top();
        mDataStack.pop();

        auto rhs = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( -( lhs == rhs ) );
      }
    }
  }){
}

//! @todo create a 'microcode' instruction set
//    to translate these tokens into for the 'real' forth machine to execute
//  The new system will take forth program instructions, and compile them into
//   virtual machine instructions.  These instructions will be what's actually
//   executed.  This architecture allows function calls to work in a more
//   realistic/real-world way.  currently the system replace a fn call with
//   that functions contents, the new method will compile the function, and
//   place it in main memory somewhere to be called later.  The new machine
//   instructions etc are necessary because terms like 'return' or any kind
//   of function return does not exist in the forth lexicon, but can exist
//   in the micro code. The idea of 'return' is necessary to exist when using
//   a regular call stack.
//  'First pass' expectiations of the contents of this micro code includes:
//  goto
//  conditional goto
//  call
//  return
//  load value
//  store value
//  push
//  pop
//  math
//
//  These instructions will enable most necessary functionality.
//
//  To implement this, functions are written to main memory, and the main
//    program is saved, then appended after parsing is complete.  This
//    ambiguates the starting point.  To remedy this, the starting point can be
//    saved at the end of the program memory space.  The main program isn't
//    being written first because fn-word addresses are not yet known.  Doing it
//    this way is easier in that respect, but a better solution should be found.
//  A potential solution to the above problem involves using an intermediate
//    language and doing a second pass to translate this into actual code, this
//    allows all function sizes to be known before writing to main memory.
//  Another solution is to do the above but in place, and not utilizing an
//    intermediate.  Instead calls to a function will be replaced with call 0,
//    call 1, call 2 etc, where each function is assigned a number; after
//    primary parsing is complete a second pass will be run to replace each
//    call with the correct address.
void
FORTH::read( const std::string& text ){
  stringstream ss( text );
  string word;
  decltype( mTokens ) newTokens;

  while( ss >> word ){
    newTokens.emplace_back( classify( word ), word );
  }

  vector<data_t> main_prog;
  map<string, address_t> fnAddresses;
  map<string, address_t> varAddresses;
  address_t prog_address_cntr = 0;
  address_t var_address_cntr = 0;

  auto translate =
  [&]( pair<TOKEN, string> tok )->data_t{
    switch( tok.first ){
    case TOKEN::FETCH:
      return u_LOAD;
    break;

    case TOKEN::STORE:
      return u_STORE;
    break;

    case TOKEN::WORD:
      if( varAddresses.count( tok.second ) > 0 ){
        return u_PUSH | ( varAddresses[tok.second] << 8 );
      } else if( fnAddresses.count( tok.second ) ){
        return u_CALL | ( fnAddresses[tok.second] << 8 );
      } else {
        throw runtime_error( string( "Unrecognized word '" ) + tok.second + "'" );
      }
    break;

    default:
      throw runtime_error( string( "invalid word during definition" ) + tok.second );
    break;
    }
  };

  for( auto iter = newTokens.begin(); iter != newTokens.end(); ++iter ){
    auto tok = *iter;

    switch( tok.first ){
    case TOKEN::DECLARE:
      varAddresses[tok.second] = var_address_cntr++;
    break;

    case TOKEN::DEFINE:
      tok = *(++iter);

      fnAddresses[tok.second] = prog_address_cntr;

      while( tok.first != TOKEN::DEFINE || tok.second != ";" ){
        mProgMem[prog_address_cntr++] = translate( *(++iter) );
      }
    break;

    case TOKEN::FETCH:
    case TOKEN::STORE:
    case TOKEN::WORD:
        main_prog.push_back( translate( tok ) );
    break;

    default:
      throw runtime_error( string( "Invalid token" ) + tok.second );
    break;
    }
  }

  for( auto word : main_prog ){
    mProgMem[prog_address_cntr++] = word;
  }
}

void
FORTH::execute(){
  while( mTokenIndex != mTokens.size() ){
    switch( mTokens[mTokenIndex].first ){
    case TOKEN::DEFINE:// word
      handle_definition();
    break;

    case TOKEN::DECLARE:// variable
      handle_declare();
    break;

    case TOKEN::FETCH:// @
      handle_fetch();
    break;

    case TOKEN::STORE:// !
      handle_store();
    break;

    case TOKEN::WORD:
      handle_word();
    break;

    case TOKEN::BRANCH:
      handle_branch();
    break;

    case TOKEN::LOOP:
      handle_loop();
    break;

    default:
      //! @todo exception?
      cout << "Unexpected token:\t" << mTokens[mTokenIndex].second << endl;
      ++mTokenIndex;
    break;
    }
  }
}

