#include<string>
#include<iostream>

#include"FORTH.hh"

using namespace std;

int main(){
  FORTH forth;
  string line;

  while( cout << ">" && getline( cin, line ) ){
    forth.read( line );

    forth.execute();

    cout << " ok" << endl;
  }

  return 0;
}

