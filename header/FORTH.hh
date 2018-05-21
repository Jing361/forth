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

using token = std::pair<TOKEN, std::string>;

class FORTH{
private:
  std::vector<data_t> mMainProg;
  std::vector<token> mTokens;
  decltype( mTokens )::iterator mTokIter;
  //! @todo put dictionary in vm memory
  // the dictionary is not being put into memory.  Putting it into memory
  //  allows for there to be a context pointer, enabling different contexts to
  //  overload words differently.
  std::map<std::string, address_t> mVarDictionary;
  std::map<std::string, data_t> mConstDictionary;
  std::map<std::string, std::vector<data_t> > mFuncDictionary;
  address_t mVariable_cntr = 0;

  void
  handle_define();

  void
  handle_primary();

  void
  handle_branch();

  void
  handle_loop();

public:
  void
  read( const std::string& text );

  std::vector<data_t>
  get_code();
};

#endif

