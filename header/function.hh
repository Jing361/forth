#ifndef __FUNCTION_HH__
#define __FUNCTION_HH__

#include<string>
#include<memory>
#include<vector>

#include"expression.hh"

class function{
private:
  std::string mName;
  std::vector<std::unique_ptr<expression> > mBody;

public:
  function( const std::string& name, std::vector<std::unique_ptr<expression> >&& body );

  std::string name() const;

  std::string generate() const;
};

#endif

