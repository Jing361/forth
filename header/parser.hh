#ifndef __PARSER_HH__
#define __PARSER_HH__

#include<vector>
#include<utility>
#include<string>
#include<map>
#include<memory>

#include"tokens.hh"

class parser{
private:
  std::vector<std::pair<TOKEN_CLASS, std::string> > mTokens;
  std::map<std::string, std::vector<expression> > mDictionary;
  decltype( mTokens )::iterator mCurTok;

  std::unique_ptr<expression> log_error( const std::string& str );

  void handle_definition();

  std::unique_ptr<function> parse_definition();

public:
  template<typename inputIter>
  void read( inputIter first, inputIter last ){
    mTokens.insert( mTokens.end(), first, last );
  }

  void addRead( TOKEN_CLASS cls, const std::string& str );

  void parse();
};

#endif

