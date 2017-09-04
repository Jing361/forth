#ifndef __FORTH_FUNCTION_HH__
#define __FORTH_FUNCTION_HH__

#include<string>
#include<memory>
#include<vector>

#include"expression.hh"

class function_f{
private:
  std::string mName;
  std::vector<std::string> mBody;

public:
  function_f( const std::string& name, std::vector<std::string>&& body );

  std::string name() const;

  std::vector<std::string> code() const;
};

#endif

