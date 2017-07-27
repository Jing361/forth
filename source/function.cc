#include<utility>

#include<function.hh>

function_f::function_f( const std::string& name, std::vector<std::unique_ptr<expression> >&& body ):
  mName( name ),
  mBody( move( body ) ){
}

std::string function_f::name() const{
  return "func name";
}

std::string function_f::generate() const{
  return "function";
}

