#include<utility>

#include<function.hh>

using namespace std;

function_f::function_f( const string& name, vector<string>&& body ):
  mName( name ),
  mBody( move( body ) ){
}

string function_f::name() const{
  return mName;
}

vector<string> function_f::code() const{
  return mBody;
}

