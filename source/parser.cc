#include<iostream>

#include"parser.hh"

using namespace std;
using namespace TOKEN_CLASS;

void parser::addRead( TOKEN_CLASS cls, const string& str ){
  mTokens.emplace_back( cls, str );
}

unique_ptr<expression> parser::log_error( const string& str ){
  cout << "Log Error: " << str << endl;
  return nullptr;
}

void parser::parse(){
  bool not_eof = true;

  mCurTok = mTokens.begin();

  while( not_eof ){
    switch( mCurTok->first ){
    case DEFINE:
      handle_definition();
    break;

    case WORD:
    break;

    case NUMBER:
    break;

    case CLASS_EOF:
      not_eof = false;
    break;

    default:
      cout << "Unexpected token:\t" << mCurTok->second << endl;
    break;
    }
  }

  mTokens.clear();
}

void parser::handle_definition(){
  if( auto word = parse_definition() ){
    cout << "Parsed definition.\n" << endl;
    mDictionary[word.name] = move( word.definition );
  } else {
    ++mCurTok;
  }
}

unique_ptr<function> parser::parse_definition(){
  if( mCurTok->second != ":" ){
    return log_error( "Bad token\t" + mCurTok->second + "\nExpected ':'\n" );
  } else {
    ++mCurTok;
  }

  while( mCurTok->second != ";" ){
  }
}

