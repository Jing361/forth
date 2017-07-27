#ifndef __NUMBER_HH__
#define __NUMBER_HH__

#include"expression.hh"

class number : public expression{
private:
  int mValue;

public:
  number( int value );

  std::string generate_code() const;
};

#endif

