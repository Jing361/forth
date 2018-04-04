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

void FORTH::handle_branch(){
  if( mTokens.front().second == "ELSE" ){
    while( mTokens.front().second != "THEN" ){
      mTokens.pop_front();
    }
  } else if( mTokens.front().second == "IF" ){
    if( !mDataStack.top() ){
      while( ( mTokens.front().second != "ELSE" )
          && ( mTokens.front().second != "THEN" ) ){
        mTokens.pop_front();
      }
    }

    mDataStack.pop();
  }

  mTokens.pop_front();
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
        while( mTokens.front().second.back() != '\"' ){
          cout << mTokens.front().second;

          mTokens.pop_front();
        }

        string last = mTokens.front().second;

        cout << last.substr( 0, last.size() - 1 );

        mTokens.pop_front();
      }
    },
    {"CR",
      [](){
        cout << endl;
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

    case TOKEN::BRANCH:
      handle_branch();
    break;

    default:
      //! @todo exception?
      cout << "Unexpected token:\t" << mTokens.front().second << endl;
      mTokens.pop_front();
    break;
    }
  }
}

