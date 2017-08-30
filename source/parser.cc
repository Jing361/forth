#include<sstream>

#include"parser.hh"

using namespace std;

void parser::addRead( TOKEN_CLASS cls, const string& str ){
  mTokens.emplace_back( cls, str );
}

void parser::parse(){
  if( mTokens.empty() ){
    return;
  }

  bool not_eof = true;

  mCurTok = mTokens.begin();

  while( not_eof ){
    switch( mCurTok->first ){
    case TOKEN_CLASS::DEFINE:
      handle_definition();
    break;

    case TOKEN_CLASS::DECLARE:
      handle_declare();
    break;

    case TOKEN_CLASS::FETCH:
      handle_fetch();
    break;

    case TOKEN_CLASS::STORE:
      handle_store();
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
      cout << "Unexpected token:\t" << ( mCurTok++ )->second << endl;
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
  if( mCurTok->second == "." ){
    cout << mStack.top() << endl;
    mStack.pop();
  } else {
    string word( mCurTok->second );

    try{
      mDictionary.at( word );
    } catch( out_of_range& ){
      try{
        auto idx = mVarIndexes.at( word );
        mStack.push( mVariables[idx] );
      } catch( out_of_range& ){
        cout << "Use of undefined word:\t" << word << endl;
      }
    }
  }

  ++mCurTok;
}

void parser::handle_math(){
  decltype( mStack )::value_type result;
  auto lhs = mStack.top();
  mStack.pop();
  auto rhs = mStack.top();
  mStack.pop();

  if( mCurTok->second == "+" ){
    result = lhs + rhs;
  } else if( mCurTok->second == "-" ){
    result = lhs - rhs;
  } else if( mCurTok->second == "*" ){
    result = lhs * rhs;
  } else if( mCurTok->second == "/" ){
    result = lhs / rhs;
  }

  mStack.push( result );

  ++mCurTok;
}

void parser::handle_number(){
  stringstream ss( ( mCurTok++ )->second );
  decltype( mStack )::value_type value;

  ss >> value;

  mStack.push( value );
}

void parser::handle_declare(){
  string word = ( ++mCurTok )->second;

  try{
    /*! @todo should probably throw error for redeclaring */
    mVarIndexes.at( word );
  } catch( out_of_range& ){
    mVarIndexes[word] = mVarIndex++;
    mVariables.push_back( 0 );
  }

  ++mCurTok;
}

void parser::handle_fetch(){
  auto index = mStack.top();

  mStack.pop();

  mStack.push( mVariables[index] );

  ++mCurTok;
}

void parser::handle_store(){
  auto index = mStack.top();

  mStack.pop();

  auto value = mStack.top();

  mStack.pop();

  mVariables[index] = value;

  ++mCurTok;
}

unique_ptr<function_f> parser::parse_definition(){
  if( mCurTok->second != ":" ){
    return log_error<function_f>( string( "Bad token\t" ) + mCurTok->second + ".\nExpected ':'\n" );
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
      body.emplace_back( move( parse_operation() ) );
    break;

    default:
      return log_error<function_f>( string( "Bad token\t" ) + mCurTok->second + "." );
    break;
    }
  }

  ++mCurTok;

  return make_unique<function_f>( name, move( body ) );
}

unique_ptr<expression> parser::parse_word(){
  string word = mCurTok->second;

  try{
    mDictionary.at( word );

    return make_unique<call>( word );
  } catch( out_of_range& ) {
    return log_error<expression>( string( "Use of undefined word:\t" ) + word + "." );
  }
}

unique_ptr<number> parser::parse_number(){
  string word( ( mCurTok++ )->second );
  stringstream ss( word );
  int value;

  if( ss >> value ){
    return make_unique<number>( value );
  } else {
    return log_error<number>( string( "Expected a number. Got:\t" ) + word + "." );
  }
}

unique_ptr<operation> parser::parse_operation(){
  string op( mCurTok->second );

  if( mCurTok->first != TOKEN_CLASS::MATH ){
    ++mCurTok;

    return log_error<operation>( string( "Expected an operator. Got:\t" ) + op + "." );
  } else {
    return make_unique<operation>( op[0] );
  }
}

