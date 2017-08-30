#include<iostream>
#include<sstream>

#include"lexer.hh"

using namespace std;

void lexer::lex( const string& command ){
  stringstream ss( command );
  string word;

  while( ss >> word ){
    TOKEN_CLASS cls = TOKEN_CLASS::NONE;

    if( word == ":" || word == ";" ){
      cls = TOKEN_CLASS::DEFINE;
    } else if( word == "@" ){
      cls = TOKEN_CLASS::FETCH;
    } else if( word == "!" ){
      cls = TOKEN_CLASS::STORE;
    } else if( isdigit( word[0] ) ){
      stringstream ns( word );
      int testNum;

      if( ns >> testNum ){
        cls = TOKEN_CLASS::NUMBER;
      }
    } else if( word == "*" || word == "/" ||
               word == "+" || word == "-" ){
      cls = TOKEN_CLASS::MATH;
    } else {
      cls = TOKEN_CLASS::WORD;
    }

    if( cls == TOKEN_CLASS::NONE ){
      cout << "Invalid token: '" << word << "'." << endl;
    } else {
      mTokens.emplace_back( cls, word );
    }
  }

  mTokens.emplace_back( TOKEN_CLASS::E_OF_F, "" );
}

void lexer::reset(){
  mTokens.clear();
}

decltype( lexer::mTokens )::iterator lexer::begin(){
  return mTokens.begin();
}

decltype( lexer::mTokens )::iterator lexer::end(){
  return mTokens.end();
}

