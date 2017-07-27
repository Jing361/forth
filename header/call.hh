#ifndef __CALL_HH__
#define __CALL_HH__

#include<string>

#include"expression.hh"

class call : public expression{
private:
  std::string mName;

public:
  call( const std::string& name );

  std::string generate_code() const;
};

#endif

