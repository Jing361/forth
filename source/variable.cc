#include<variable.hh>

variable::variable( const std::string& name, ACTION action, int address ):
  mName( name ),
  mAction( action ),
  mAddress( address ){
}

std::string variable::generate_code() const{
  return "WHOA";
}

