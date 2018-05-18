#include<string>
#include<iostream>

#include"FORTH.hh"

using namespace std;

int main(){
  FORTH forth;
  string line;

  while( cout << ">" && getline( cin, line ) ){
    forth.read( line );

    //virtual_machine vm( forth.get_code() );

    cout << " ok" << endl;
  }

  return 0;
}

