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
  auto def = parse_definition();

  if( !def.code().empty() ){
    cout << "Parsed definition.\n" << endl;
    mDictionary[def.name()] = move( def.code() );
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

function_f parser::parse_definition(){
  if( mCurTok->second != ":" ){
    cout << "Bod token:\t" << mCurTok->second << "\nExpected:\t':'" << endl;
    return function_f( "", vector<string>() );
  } else {
    ++mCurTok;
  }

  string name( ( mCurTok++ )->second );
  vector<string> body;

  while( mCurTok->second != ";" ){
    body.push_back( ( mCurTok++ )->second );
  }

  ++mCurTok;

  return function_f( name, move( body ) );
}

