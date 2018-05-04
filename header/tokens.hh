#ifndef __TOKENS_HH__
#define __TOKENS_HH__

enum class TOKEN{
  NONE,
  STORE,
  FETCH,
  DEFINE,
  WORD,
  NUMBER,
  DECLARE,
  BRANCH,
  LOOP,
  E_OF_F,
};

enum MICRO_CODE{
  u_JUMP,
  u_JUMP_Z,
  u_CALL,
  u_RETURN,
  u_LOAD,
  u_STORE,
  u_PUSH,
  u_POP,
};

#endif

