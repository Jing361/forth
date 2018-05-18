#ifndef __FORTH_HH__
#define __FORTH_HH__

#include<string>
#include<stack>
#include<vector>
#include<utility>
#include<functional>
#include<map>

#include<tokens.hh>

using address_t = unsigned long;
using data_t = long;

class io_controller{
public:
  void
  display( data_t );

  void
  display( address_t );

  data_t
  read();
};

class dictionary{
public:
  dictionary( std::array<data_t, 1024>& mem_ref );

  void
  insert( std::string );

  void
  insert( std::string, address_t );

  void
  insert( std::string, std::vector<data_t> );
};

class virtual_machine{
private:
  /*! @todo it's possible to merge the data and call stack and main memory all
   * together.  any merging will basically change the merged part into a pointer
   * to main memory.
   */
  std::stack<data_t> mDataStack;
  std::stack<data_t> mCallStack;
  std::array<data_t, 1024> mMainMem;
  address_t mPC;
  io_controller mIO;

public:
  virtual_machine( decltype( mMainMem ) data )
    : mMainMem( data ){
  }

  void
  cycle();
};

class FORTH{
private:
  std::vector<data_t> mMainProg;
  std::map<std::string, address_t> mVarDictionary;
  std::map<std::string, data_t> mConstDictionary;
  std::map<std::string, std::vector<data_t> > mFuncDictionary;
  address_t mVariable_cntr = 0;

  std::vector<data_t>
  handle_define( std::vector<std::pair<TOKEN, std::string> >::iterator& iter );

  void
  handle_branch( std::vector<std::pair<TOKEN, std::string> >::iterator& iter );

  void
  handle_loop( std::vector<std::pair<TOKEN, std::string> >::iterator& iter );

  void handle_primary();

public:
  FORTH();

  void
  read( const std::string& text );

  std::vector<data_t>
  get_code();
};

#endif

