#include<number.hh>

number::number( int value ):
  mValue( value ){
}

std::string number::generate_code() const{
  return "number";
}

