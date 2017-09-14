#include<string>

#include"lexer.hh"
#include"parser.hh"

using namespace std;

int main(){
  parser pars;
  string line;

  while( cout << ">" && getline( cin, line ) ){
    lexer lex;

    lex.lex( line );

    pars.read( lex.begin(), lex.end() );

    pars.parse();
  }

  return 0;
}

