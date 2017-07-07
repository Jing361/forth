#include"lexer.hh"
#include"parser.hh"

using namespace std;

int main(){
  lexer lex;
  parser pars;

  lex.lex( "" );

  pars.read( lex.begin(), lex.end() );

  pars.parse();

  return 0;
}

