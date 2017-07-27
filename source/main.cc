#include<string>

#include"lexer.hh"
#include"parser.hh"

using namespace std;

int main(){
  lexer lex;
  parser pars;
  string line;

  while( cout << ">" && cin >> line ){
    lex.lex( line );

    pars.read( lex.begin(), lex.end() );

    pars.parse();
  }

  return 0;
}

