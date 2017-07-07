#include<iostream>
#include<sstream>

#include"parser.hh"

using namespace std;

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
    case TOKEN_CLASS::DEFINE:
      handle_definition();
    break;

    case TOKEN_CLASS::WORD:
      handle_word();
    break;

    case TOKEN_CLASS::NUMBER:
      handle_number();
    break;

    case TOKEN_CLASS::MATH:
      handle_math();
    break;

    case TOKEN_CLASS::E_OF_F:
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
  if( auto def = parse_definition() ){
    cout << "Parsed definition.\n" << endl;
    mDictionary[def->name()] = move( def );
  } else {
    ++mCurTok;
  }
}

void parser::handle_word(){
  if( auto word = parse_word() ){
    cout << "Parsed word.\n" << endl;
    mActions.emplace_back( word );
  } else {
    ++mCurTok;
  }
}

void parser::handle_math(){
  if( auto op = parse_operation() ){
    cout << "Parsed operation.\n" << endl;
    mActions.emplace_back( op );
  } else {
    ++mCurTok;
  }
}

void parser::handle_number(){
  if( auto num = parse_number() ){
    cout << "Parsed number.\n" << endl;
    mActions.emplace_back( num );
  } else {
    ++mCurTok;
  }
}

unique_ptr<function> parser::parse_definition(){
  if( mCurTok->second != ":" ){
    return log_error( string( "Bad token\t" ) + mCurTok->second + ".\nExpected ':'\n" );
  } else {
    ++mCurTok;
  }

  string name( ( mCurTok++ )->second );
  vector<unique_ptr<expression> > body;

  while( mCurTok->second != ";" ){
    switch( mCurTok->first ){
    case TOKEN_CLASS::WORD:
      body.emplace_back( move( parse_word() ) );
    break;

    case TOKEN_CLASS::NUMBER:
      body.emplace_back( move( parse_number() ) );
    break;

    case TOKEN_CLASS::MATH:
      body.emplace_back( move( parse_math() ) );
    break;

    default:
      return log_error( string( "Bad token\t" ) + mCurTok->second + "." );
    break;
    }
  }

  return make_unique<function>( name, move( body ) );
}

unique_ptr<expression> parser::parse_word(){
  string word = ( mCurTok++ )->second;

  if( mCurTok->first != TOKEN_CLASS::WORD ){
    return log_error( string( "Invalid token:\t" ) + word + "\nExpected a word." );
  }

  if( mCurTok->first == TOKEN_CLASS::STORE ){
    try{
      mVariables.at( word );
    } catch( out_of_range& ) {
      mVariables[word] = mVarIndex++;
    }

    ++mCurTok;

    return make_unique<variable>( word, ACTION::STORE, mVariables[word] );
  } else if( mCurTok->first == TOKEN_CLASS::FETCH ){
    try{
      mVariables.at( word );

      ++mCurTok;

      return make_unique<variable>( word, ACTION::FETCH, mVariables[word] );
    } catch( out_of_range& ) {
      return log_error( string( "Cannot fetch unset variable:\t" ) + word + "." );
    }
  } else {
    try{
      mDictionary.at( word );

      return make_unique<call>( word );
    } catch( out_of_range& ) {
      return log_error( string( "Use of undefined word:\t" ) + word + "." );
    }
  }
}

unique_ptr<number> parser::parse_number(){
  string word( ( mCurTok++ )->second );
  stringstream ss( word );
  int value;

  if( ss >> value ){
    return make_unique<number>( value );
  } else {
    return log_error( string( "Expected a number.  Got:\t" ) + word "." );
  }
}

unique_ptr<operation> parser::parse_operation(){
  string op( mCurTok->second );

  if( mCurTok->first != TOKEN_CLASS::MATH ){
    ++mCurTok;

    return log_error( string( "Expected an operator.  Got:\t" ) + op + "." );
  } else {
    return make_unique<operation>( op[0] );
  }
}

