#include<iostream>
#include<sstream>

#include"lexer.hh"

using namespace std;
using namespace TOKEN_CLASS;

void lexer::lex( const string& command ){
  stringstream ss( command );
  string word;

  while( ss >> word ){
    TOKEN_CLASS cls = NONE;

    if( word == ":" || word == ";" ){
      cls = DEFINE;
    } else if( word == "@" ){
      cls = FETCH;
    } else if( word == "!" ){
      cls = STORE;
    } else if( isdigit( word[0] ) ){
      bool isNum = true;

      for( auto digit : word ){
        isNum = isNum && isdigit( digit );
      }

      if( isNum ){
        cls = NUMBER;
      }
    } else if( word == "*" || word == "/" ||
               word == "+" || word == "-" ){
      cls = MATH;
    } else {
      cls = WORD;
    }

    if( cls == NONE ){
      cout << "Invalid token: '" << word << "'." << endl;
    }
  }

  mTokens.emplace_back( EOF, "" );
}

decltype( lexer::mTokens )::iterator begin(){
  return mTokens.begin();
}

decltype( lexer::mTokens )::iterator end(){
  return mTokens.end();
}

