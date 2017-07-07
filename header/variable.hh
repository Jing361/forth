#ifndef __VARIABLE_HH__
#define __VARIABLE_HH__

#include<string>

#include"expression.hh"

enum class ACTION{
  FETCH,
  STORE,
};

class variable : public expression{
private:
  std::string mName;
  ACTION mAction;
  int mAddress;

public:
  variable( const std::string& name, ACTION action, int address );
};

#endif

