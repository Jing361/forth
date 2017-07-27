#include<operation.hh>

operation::operation( char op ):
  mOp( op ){
}

std::string operation::generate_code() const{
  return "operation";
}

