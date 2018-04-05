#include<vector>
#include<sstream>
#include<iostream>

#include<FORTH.hh>

using namespace std;

static TOKEN classify( const std::string& word ){
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

void FORTH::handle_definition(){
  ++mTokenIter;

  auto word = mTokenIter->second;
  vector<string> tokens;

  ++mTokenIter;

  while( mTokenIter->second != ";" ){
    tokens.emplace_back( mTokenIter->second );

    ++mTokenIter;
  }

  /* If the program looks like this:
   *
   * 1
   * X
   * 4<mTokenIter
   *
   * Where X is a word defined as : X 2 3 ;
   * then the process to execute X looks like this:
   * 1
   * X<nextIter
   * 4<mTokenIter
   *
   * 1
   * X<nextIter
   * 2
   * 3
   * 4<mTokenIter
   *
   * 1
   * X<nextIter
   * 2<mTokenIter
   * 3
   * 4
   *
   * 1
   * 2<mTokenIter
   * 3
   * 4
   *
   */
  mDictionary[word] =
    [this,tokens](){
      auto nextIter = mTokenIter;
      --nextIter;

      for( auto it = tokens.begin(); it != tokens.end(); ++it ){
        mTokens.emplace( mTokenIter, classify( *it ), *it );
      }

      mTokenIter = nextIter;
      ++mTokenIter;

      mTokens.erase( nextIter );
    };

  ++mTokenIter;
}

void FORTH::handle_declare(){
  ++mTokenIter;

  mDictionary[mTokenIter->second] =
    [this](){
      mDataStack.push( mAddressCounter );
    };

  mAddressCounter++;

  ++mTokenIter;
}

void FORTH::handle_fetch(){
  auto index = mDataStack.top();

  mDataStack.pop();

  mDataStack.push( mMainMem[index] );

  ++mTokenIter;
}

void FORTH::handle_store(){
  auto index = mDataStack.top();

  mDataStack.pop();

  mMainMem[index] = mDataStack.top();

  mDataStack.pop();

  ++mTokenIter;
}

void FORTH::handle_word(){
  auto word = mTokenIter->second;
  int value;
  stringstream ss( word );

  ++mTokenIter;

  if( ss >> value ){
    mDataStack.push( value );
  } else {
    mDictionary.at( word )();
  }
}

void FORTH::handle_branch(){
  if( mTokenIter->second == "ELSE" ){
    while( mTokenIter->second != "THEN" ){
      ++mTokenIter;
    }
  } else if( mTokenIter->second == "IF" ){
    if( !mDataStack.top() ){
      while( ( mTokenIter->second != "ELSE" )
          && ( mTokenIter->second != "THEN" ) ){
        ++mTokenIter;
      }
    }

    mDataStack.pop();
  }

  ++mTokenIter;
}

void FORTH::handle_loop(){
  // call stack is used for loop control variables
  // this would look nicer if we could just store the iterator at do
  if( mTokenIter->second == "LOOP" ){
    auto lcv = mCallStack.top();
    mCallStack.pop();
    auto limit = mCallStack.top();
    ++lcv;

    if( lcv < limit ){
      while( ( --mTokenIter )->second != "DO" ){
      }

      ++mTokenIter;
      mCallStack.push( lcv );
    } else {
      // we're done, pop off limit
      mCallStack.pop();
      ++mTokenIter;
    }
  } else if( mTokenIter->second == "DO" ){
    auto lcv = mDataStack.top();
    mDataStack.pop();
    auto limit = mDataStack.top();
    mDataStack.pop();

    mCallStack.push( limit );
    mCallStack.push( lcv );

    ++mTokenIter;
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
        while( mTokenIter->second.back() != '\"' ){
          cout << mTokenIter->second;

          ++mTokenIter;
        }

        string last = mTokenIter->second;

        cout << last.substr( 0, last.size() - 1 );

        ++mTokenIter;
      }
    },
    {"CR",
      [](){
        cout << endl;
      }
    },
    {"I",
      [this](){
        cout << mCallStack.top();
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

void FORTH::read( const std::string& text ){
  stringstream ss( text );
  string word;
  decltype( mTokens ) newTokens;

  while( ss >> word ){
    newTokens.emplace_back( classify( word ), word );
  }

  if( mTokenIter == mTokens.end() ){
    --mTokenIter;

    mTokens.insert( mTokens.end(), newTokens.begin(), newTokens.end() );

    ++mTokenIter;
  }
}

void FORTH::execute(){
  while( mTokenIter != mTokens.end() ){
    switch( mTokenIter->first ){
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
      cout << "Unexpected token:\t" << mTokenIter->second << endl;
      ++mTokenIter;
    break;
    }
  }
}

