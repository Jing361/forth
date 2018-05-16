#include<vector>
#include<sstream>
#include<iostream>

#include<FORTH.hh>

using namespace std;

static TOKEN
classify( const std::string& word ){
  stringstream ss( word );
  data_t num;

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
  } else if( ss >> num ){
    return TOKEN::NUMBER;
  } else {
    return TOKEN::WORD;
  }
}

FORTH::FORTH()
  : mBuiltins( { {".", bi_PRINT_TOP}, {"CR", bi_CR}, {"I", bi_I},
                 {"EMIT", bi_EMIT}, {"DUP", bi_DUP}, {"SWAP", bi_SWAP},
                 {".S", bi_PRINT_STACK}, {"OVER", bi_OVER}, {"DROP", bi_DROP},
                 {"ROT", bi_ROT}, {".\"", bi_PRINT_STRING} } )
  , mBuiltinops( {
    {bi_CR,
    [this](){
      cout << endl;
    }},
    {bi_I,
    [this](){
      cout << mCallStack.top();
    }},
    {bi_EMIT,
    [this](){
      cout << char( mDataStack.top() & 0xFF );

      mDataStack.pop();
    }},
    {bi_DUP,
    [this](){
      mDataStack.push( mDataStack.top() );
    }},
    {bi_SWAP,
    [this](){
      auto first = mDataStack.top();
      mDataStack.pop();
      auto second = mDataStack.top();
      mDataStack.pop();

      mDataStack.push( first );
      mDataStack.push( second );
    }},
    {bi_PRINT_TOP,
    [this](){
      cout << mDataStack.top();
    }},
    {bi_PRINT_STACK,
    [this](){
      auto stack_cp = mDataStack;

      while( !stack_cp.empty() ){
        cout << stack_cp.top() << ' ';
        stack_cp.pop();
      }
    }},
    {bi_PRINT_STRING,
    [this](){
      auto count = mDataStack.top();
      mDataStack.pop();
      auto address = mDataStack.top();
      mDataStack.pop();

      for( int i = 0; i < count; ++i ){
        cout << char( mMainMem[address + i] & 0xFF );
      }
    }},
    {bi_OVER,
    [this](){
      auto top = mDataStack.top();
      mDataStack.pop();

      auto two = mDataStack.top();

      mDataStack.push( top );
      mDataStack.push( two );
    }},
    {bi_DROP,
    [this](){
      mDataStack.pop();
    }},
    {bi_ROT,
    [this](){
    }}
  } ){
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
//
//  This implementation does not allow for an interactive mode of operation.
void
FORTH::read( const std::string& text ){
  stringstream ss( text );
  string word;
  vector<pair<TOKEN, string> > newTokens;

  while( ss >> word ){
    newTokens.emplace_back( classify( word ), word );
  }

  vector<data_t> main_prog;
  map<string, address_t> fnAddresses;
  map<string, address_t> varAddresses;
  map<string, address_t> stringAddresses;
  address_t prog_address_cntr = 0;
  address_t var_address_cntr = 0;
  address_t jump_address;

  auto translate =
  [&]( pair<TOKEN, string> tok )->data_t{
    stringstream ss( tok.second );
    data_t num;

    switch( tok.first ){
    case TOKEN::FETCH:
      return u_LOAD;
    break;

    case TOKEN::STORE:
      return u_STORE;
    break;

    case TOKEN::BRANCH:
      //! @todo calculate jump_address
      return u_JUMP_Z | jump_address;
    break;

    case TOKEN::LOOP:
      //! @todo calculate jump_address
      return u_JUMP_Z | jump_address;
    break;

    case TOKEN::NUMBER:
      ss >> num;
      return u_PUSH | int( num ) << 8 ;
    break;

    case TOKEN::WORD:
      if( varAddresses.count( tok.second ) > 0 ){
        return u_PUSH | ( varAddresses[tok.second] << 8 );
      } else if( fnAddresses.count( tok.second ) ){
        return u_CALL | ( fnAddresses[tok.second] << 8 );
      } else if( stringAddresses.count( tok.second ) ){
        return u_PUSH | ( stringAddresses[tok.second] << 8 );
      } else if( mBuiltins.count( tok.second ) > 0 ){
        if( tok.second == ".\"" ){
        }
        return u_BUILTIN | ( mBuiltins[tok.second] );
      } else {
        throw runtime_error( string( "Unrecognized word '" ) + tok.second + "'" );
      }
    break;

    default:
      throw runtime_error( string( "invalid word during definition " ) + tok.second );
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
      ++iter;
      fnAddresses[iter->second] = prog_address_cntr;

      ++iter;
      while( iter->first != TOKEN::DEFINE || iter->second != ";" ){
        mProgMem[prog_address_cntr++] = translate( *iter++ );
      }
    break;

    case TOKEN::FETCH:
    case TOKEN::STORE:
    case TOKEN::BRANCH:
    case TOKEN::LOOP:
    case TOKEN::NUMBER:
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
  decltype( mDataStack )::value_type one;
  decltype( mDataStack )::value_type two;

  for( mAddressCounter = 0; mAddressCounter < mProgMem.size(); ++mAddressCounter ){
    auto instruction = mProgMem[mAddressCounter];
    decltype( mDataStack )::value_type index;

    switch( instruction & 0xFF ){
      case u_JUMP:
        mAddressCounter = ( instruction >> 8 );
      break;
  
      case u_JUMP_Z:
        if( mDataStack.top() == 0 ){
          mAddressCounter = ( instruction >> 8 );
        }

        mDataStack.pop();
      break;
  
      case u_CALL:
        mCallStack.push( mAddressCounter + 1 );
      break;
  
      case u_RETURN:
        mAddressCounter = mCallStack.top();
        mCallStack.pop();
      break;
  
      case u_LOAD:
        mDataStack.push( mMainMem[mDataStack.top()] );
        mDataStack.pop();
      break;
  
      case u_STORE:
        index = mDataStack.top();

        mMainMem[index] = mDataStack.top();
        mDataStack.pop();
      break;
  
      case u_PUSH:
        mDataStack.push( instruction>> 8 );
      break;
  
      case u_POP:
        mDataStack.pop();
      break;

      case u_ADD:
        one = mDataStack.top();
        mDataStack.pop();
        two = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( one + two );
      break;

      case u_SUB:
        one = mDataStack.top();
        mDataStack.pop();
        two = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( one - two );
      break;

      case u_MUL:
        one = mDataStack.top();
        mDataStack.pop();
        two = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( one * two );
      break;

      case u_DIV:
        one = mDataStack.top();
        mDataStack.pop();
        two = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( one / two );
      break;

      case u_LT:
        one = mDataStack.top();
        mDataStack.pop();
        two = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( -( one < two ) );
      break;

      case u_GT:
        one = mDataStack.top();
        mDataStack.pop();
        two = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( -( one > two ) );
      break;

      case u_CMP:
        one = mDataStack.top();
        mDataStack.pop();
        two = mDataStack.top();
        mDataStack.pop();

        mDataStack.push( -( one == two ) );
      break;

      case u_BUILTIN:
        mBuiltinops[BUILT_INS( ( instruction >> 8 ) & 0xFF )]();
      break;

      default:
        throw runtime_error( "Bad instruction" );
      break;
    }
  }
}

