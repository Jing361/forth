#ifndef __LEXER_HH__
#define __LEXER_HH__

#include<vector>
#include<utility>
#include<string>

#include"tokens.hh"

class lexer{
private:
  std::vector<std::pair<TOKEN_CLASS, std::string> > mTokens;

public:
  void lex( const std::string& command );

  decltype( mTokens )::iterator begin();
  decltype( mTokens )::iterator end();
};

#endif

