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
  } else {
    return TOKEN::WORD;
  }
}

void FORTH::handle_definition(){
  mTokens.pop_front();

  auto word = mTokens.front().second;
  vector<string> tokens;

  mTokens.pop_front();

  while( mTokens.front().second != ";" ){
    tokens.emplace_back( mTokens.front().second );

    mTokens.pop_front();
  }

  mDictionary[word] =
    [this,tokens](){
      for( auto it = tokens.rbegin(); it != tokens.rend(); ++it ){
        mTokens.emplace( mTokens.cbegin(), classify( *it ), *it );
      }
    };

  mTokens.pop_front();
}

void FORTH::handle_declare(){
  mTokens.pop_front();

  mDictionary[mTokens.front().second] =
    [this](){
      mDataStack.push( mAddressCounter );
    };

  mAddressCounter++;

  mTokens.pop_front();
}

void FORTH::handle_fetch(){
  auto index = mDataStack.top();

  mDataStack.pop();

  mDataStack.push( mMainMem[index] );

  mTokens.pop_front();
}

void FORTH::handle_store(){
  auto index = mDataStack.top();

  mDataStack.pop();

  mMainMem[index] = mDataStack.top();

  mDataStack.pop();

  mTokens.pop_front();
}

void FORTH::handle_word(){
  auto word = mTokens.front().second;
  int value;
  stringstream ss( word );

  mTokens.pop_front();

  if( ss >> value ){
    mDataStack.push( value );
  } else {
    mDictionary.at( word )();
  }
}

FORTH::FORTH(){
  mDictionary["."] = [this](){
    cout << mDataStack.top();

    mDataStack.pop();
  };

  mDictionary["CR"] = [](){
    cout << endl;
  };

  mDictionary["EMIT"] = [this](){
    cout << ( char ) mDataStack.top();

    mDataStack.pop();
  };

  mDictionary["+"] = [this](){
    auto lhs = mDataStack.top();
    mDataStack.pop();

    auto rhs = mDataStack.top();
    mDataStack.pop();

    mDataStack.push( lhs + rhs );
  };

  mDictionary["-"] = [this](){
    auto lhs = mDataStack.top();
    mDataStack.pop();

    auto rhs = mDataStack.top();
    mDataStack.pop();

    mDataStack.push( lhs - rhs );
  };

  mDictionary["*"] = [this](){
    auto lhs = mDataStack.top();
    mDataStack.pop();

    auto rhs = mDataStack.top();
    mDataStack.pop();

    mDataStack.push( lhs * rhs );
  };

  mDictionary["/"] = [this](){
    auto lhs = mDataStack.top();
    mDataStack.pop();

    auto rhs = mDataStack.top();
    mDataStack.pop();

    mDataStack.push( lhs / rhs );
  };

  mDictionary[">"] = [this](){
    auto lhs = mDataStack.top();
    mDataStack.pop();

    auto rhs = mDataStack.top();
    mDataStack.pop();

    mDataStack.push( lhs > rhs );
  };

  mDictionary["<"] = [this](){
    auto lhs = mDataStack.top();
    mDataStack.pop();

    auto rhs = mDataStack.top();
    mDataStack.pop();

    mDataStack.push( lhs < rhs );
  };

  mDictionary["DUP"] = [this](){
    mDataStack.push( mDataStack.top() );
  };
}

void FORTH::read( const std::string& text ){
  stringstream ss( text );
  string word;

  while( ss >> word ){
    mTokens.emplace( mTokens.end(), classify( word ), word );
  }
}

void FORTH::execute(){
  while( !mTokens.empty() ){
    switch( mTokens.front().first ){
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

    default:
      //! @todo exception?
      cout << "Unexpected token:\t" << mTokens.front().second << endl;
      mTokens.pop_front();
    break;
    }
  }
}

