#ifndef __OPERATION_HH__
#define __OPERATION_HH__

#include"expression.hh"

class operation : public expression{
private:
  char mOp;

public:
  operation( char op );
};

#endif

