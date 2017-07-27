#include<call.hh>

using namespace std;

call::call( const string& name ):
  mName( name ){
}

string call::generate_code() const{
  return "CALL";
}

