#ifndef __PARSER_HH__
#define __PARSER_HH__

#include<vector>
#include<utility>
#include<string>
#include<map>
#include<memory>

#include"tokens.hh"
#include"expression.hh"
#include"function.hh"
#include"number.hh"

class parser{
private:
  std::vector<expression> mActions;
  std::vector<std::pair<TOKEN_CLASS, std::string> > mTokens;
  std::map<std::string, function> mDictionary;
  std::map<std::string, int> mVariables;
  decltype( mTokens )::iterator mCurTok;
  int mVarIndex = 0;

  std::unique_ptr<expression> log_error( const std::string& str );

  void handle_definition();
  void handle_word();
  void handle_math();
  void handle_number();

  std::unique_ptr<function> parse_definition();
  std::unique_ptr<expression> parse_word();
  std::unique_ptr<number> parse_number();
  std::unique_ptr<operation> parse_operation();

public:
  template<typename inputIter>
  void read( inputIter first, inputIter last ){
    mTokens.insert( mTokens.end(), first, last );
  }

  void addRead( TOKEN_CLASS cls, const std::string& str );

  void parse();
};

#endif

