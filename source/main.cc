#include"lexer.hh"
#include"parser.hh"

using namespace std;

int main(){
  lexer lex;
  parser pars;

  lex.lex( "" );

  pars.parse( lex.begin(), lex.end() );

  return 0;
}

