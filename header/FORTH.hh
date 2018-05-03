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
  /*! @todo mTokens should be translated to be in a regular memory space
   *  as array<data_t, 512> mProgMem.  This is more representative of real
   *  forth hardware, where instructions are encoded as numbers etc. This also
   *  requires overhaul of parsing system.
   */
  std::vector<std::pair<TOKEN, std::string> > mTokens;
  std::stack<data_t> mDataStack;
  std::stack<data_t> mCallStack;
  std::array<data_t, 256> mMainMem;
  std::map<std::string, std::function<void()> > mDictionary;
  int mAddressCounter = 0;
  size_t mTokenIndex = 0;

  void handle_definition();
  void handle_declare();
  void handle_fetch();
  void handle_store();
  void handle_word();
  void handle_branch();
  void handle_loop();

public:
  FORTH();

  void read( const std::string& text );
  void execute();
};

#endif

