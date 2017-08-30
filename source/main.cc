#include<string>

#include"lexer.hh"
#include"parser.hh"

using namespace std;

int main(){
  lexer lex;
  parser pars;
  string line;

  while( cout << ">" && getline( cin, line ) ){
    lex.lex( line );

    pars.read( lex.begin(), lex.end() );

    pars.parse();

    lex.reset();
  }

  return 0;
}

