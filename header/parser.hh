#ifndef __PARSER_HH__
#define __PARSER_HH__

#include<iostream>
#include<vector>
#include<utility>
#include<string>
#include<map>
#include<memory>
#include<stack>

#include"tokens.hh"
#include"expression.hh"
#include"function.hh"
#include"number.hh"
#include"operation.hh"
#include"variable.hh"
#include"call.hh"

class parser{
private:
  std::vector<std::unique_ptr<expression> > mActions;
  std::vector<std::pair<TOKEN_CLASS, std::string> > mTokens;
  std::map<std::string, std::unique_ptr<function_f> > mDictionary;
  std::map<std::string, int> mVarIndexes;
  std::vector<long> mVariables;
  decltype( mTokens )::iterator mCurTok;
  int mVarIndex = 0;
  std::stack<long> mStack;

  template<typename T>
  std::unique_ptr<T> log_error( const std::string& str ){
    std::cout << "Log Error: " << str << std::endl;
    return nullptr;
  }

  void handle_definition();
  void handle_word();
  void handle_math();
  void handle_number();
  void handle_declare();
  void handle_fetch();
  void handle_store();

  std::unique_ptr<function_f> parse_definition();
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

