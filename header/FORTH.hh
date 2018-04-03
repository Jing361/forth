#ifndef __FORTH_HH__
#define __FORTH_HH__

#include<string>
#include<stack>
#include<queue>
#include<utility>
#include<functional>
#include<map>

#include<tokens.hh>

using address_t = unsigned long;
using data_t = long;

class FORTH{
private:
  /*! @todo it's possible to merge the data and call stack and main memory all
   * together.  any merging will basically change the merged part into a pointer
   * to main memory.
   */
  std::queue<std::pair<TOKEN, std::string> > mTokens;
  std::stack<address_t> mDataStack;
  std::stack<address_t> mCallStack;
  std::array<data_t, 256> mMainMem;
  std::map<std::string, std::function<void()> > mDictionary;
  int mAddressCounter = 0;

  void handle_definition();
  void handle_declare();
  void handle_fetch();
  void handle_store();
  void handle_word();

public:
  FORTH();

  void read( const std::string& text );
  void execute();
};

#endif

