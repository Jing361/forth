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

/*! @todo current token may want to be tracked as an iterator so that code is
 * not disposed of when it may be in a loop or conditional
 */
class FORTH{
private:
  /*! @todo it's possible to merge the data and call stack and main memory all
   * together.  any merging will basically change the merged part into a pointer
   * to main memory.
   */
  std::stack<data_t> mDataStack;
  std::stack<data_t> mCallStack;
  std::array<data_t, 1024> mMainMem;
  std::array<data_t, 256> mProgMem;
  unsigned int mAddressCounter = 0;
  size_t mTokenIndex = 0;
  std::map<std::string, BUILT_INS> mBuiltins;
  std::map<BUILT_INS, std::function<void()> > mBuiltinops;

  void
  handle_words( const std::string& tok );

public:
  FORTH();

  void read( const std::string& text );
  void execute();
};

#endif

