#ifndef __EXPRESSION_HH__
#define __EXPRESSION_HH__

#include<string>

class expression{
public:
  virtual ~expression() {}

  virtual std::string generate_code() const = 0;
};

#endif

