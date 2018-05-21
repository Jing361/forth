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
  } else if( word == "CONSTANT" ){
    return TOKEN::CONSTANT;
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

  while( ss >> word ){
    mTokens.emplace_back( classify( word ), word );
  }

  for( mTokIter = mTokens.begin(); mTokIter != mTokens.end(); ++mTokIter ){
    switch( mTokIter->first ){
      case TOKEN::STORE:
      case TOKEN::FETCH:
      case TOKEN::WORD:
      case TOKEN::NUMBER:
      case TOKEN::DECLARE:
      case TOKEN::BRANCH:
      case TOKEN::LOOP:
        handle_primary();
      break;

      case TOKEN::DEFINE:
        handle_define();
      break;

      default:
        throw runtime_error( "BAD TOKEN, GO AWAY" );
      break;
    }
  }
}

void
FORTH::handle_primary(){
  stringstream ss;
  data_t num;

  switch( mTokIter->first ){
    case TOKEN::STORE:
      mMainProg.push_back( u_STORE );
    break;

    case TOKEN::FETCH:
      mMainProg.push_back( u_FETCH );
    break;

    case TOKEN::WORD:
      if( mFuncDictionary.count( mTokIter->second ) > 0 ){
        mMainProg.push_back( u_CALL );
        mMainProg.push_back( mFuncDictionary[mTokIter->second] );
      } else if( mVarDictionary.count( mTokIter->second ) > 0 ){
        mMainProg.push_back( u_LIT );
        mMainProg.push_back( mVarDictionary[mTokIter->second] );
      } else if( mConstDictionary.count( mTokIter->second ) > 0 ){
        mMainProg.push_back( u_LIT );
        mMainProg.push_back( mConstDictionary[mTokIter->second] );
      }
    break;

    case TOKEN::NUMBER:
      ss << mTokIter->second;
      ss >> num;

      mMainProg.push_back( u_LIT );
      mMainProg.push_back( num );
    break;

    case TOKEN::DECLARE:
      ++mTokIter;
      mVarDictionary[iter->second] = mVariable_cntr++;
    break;

    case TOKEN::BRANCH:
      handle_branch();
    break;

    case TOKEN::LOOP:
      handle_loop();
    break;

    default:
      throw runtime_error( "BAD TOKEN, GO AWAY" );
    break;
  }
}

void
FORTH::handle_branch(){
  mMainProg.push_back( u_BRANCH );
  auto branch_address = mMainProg.size();
  mMainProg.push_back( 0 );

  ++mTokIter;

  while( mTokIter->second != "THEN" && mTokIter->second != "ELSE" ){
    handle_primary();
  }

  if( mTokIter->second == "THEN" ){
  } else if( mTokIter->second == "ELSE" ){
  }

  ++mTokIter;

  //off by one?
  mMainProg[branch_address] = mMainProg.size();
}

void
FORTH::handle_loop(){
}

void
virtual_machine::cycle(){
  decltype( mDataStack )::value_type one;
  decltype( mDataStack )::value_type two;

  switch( mMainMem[mPC] ){
    case u_STORE:
      one = mDataStack.top();
      mDataStack.pop();

      mMainMem[one] = mDataStack.top();
      mDataStack.pop();
    break;

    case u_ADD:
      one = mDataStack.top();
      mDataStack.pop();

      mDataStack.top() += one;
    break;

    case u_SUB:
      one = mDataStack.top();
      mDataStack.pop();

      mDataStack.top() -= one;
    break;

    case u_AND:
      one = mDataStack.top();
      mDataStack.pop();

      mDataStack.top() += one;
    break;

    case u_OR:
      one = mDataStack.top();
      mDataStack.pop();

      mDataStack.top() |= one;
    break;

    case u_XOR:
      one = mDataStack.top();
      mDataStack.pop();

      mDataStack.top() ^= one;
    break;

    case u_PUSH_CALL:
      mCallStack.push( mDataStack.top() );
      mDataStack.pop();
    break;

    case u_FETCH:
      one = mDataStack.top();

      mDataStack.top() = mMainMem[one];
    break;

    case u_DROP:
      mDataStack.pop();
    break;

    case u_DUP:
      mDataStack.push( mDataStack.top() );
    break;

    case u_OVER:
      one = mDataStack.top();
      mDataStack.pop();

      two = mDataStack.top();

      mDataStack.push( one );
      mDataStack.push( two );
    break;

    case u_POP_CALL:
      mDataStack.push( mCallStack.top() );
      mCallStack.pop();
    break;

    case u_SWAP:
      one = mDataStack.top();
      mDataStack.pop();

      two = mDataStack.top();

      mDataStack.top() = one;
      mDataStack.push( two );
    break;

    //! @todo add a goto?
    case u_BRANCH:
      ++mPC;

      if( mDataStack.top() == 0 ){
        mPC = mMainMem[mPC];
      }

      mDataStack.pop();
    break;

    case u_CALL:
      ++mPC;
      mCallStack.push( mPC );

      mPC = mMainMem[mPC];
    break;

    case u_EXIT:
      mPC = mCallStack.top();
      mCallStack.pop();
    break;

    case u_LIT:
      ++mPC;
      mDataStack.push( mMainMem[mPC] );
    break;

    default:
      throw runtime_error( "Bad instruction" );
    break;
  }

  ++mPC;
}

