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

enum u_CODE{
  u_JUMP,
  u_JUMP_Z,
  u_CALL,
  u_RETURN,
  u_LOAD,
  u_STORE,
  u_PUSH,
  u_POP,
  u_ADD,
  u_SUB,
  u_MUL,
  u_DIV,
  u_LT,
  u_GT,
  u_CMP,
  u_BUILTIN,
};

enum BUILT_INS{
  bi_CR,
  bi_I,
  bi_EMIT,
  bi_DUP,
  bi_SWAP,
  bi_PRINT_TOP,
  bi_PRINT_STACK,
  bi_PRINT_STRING,
  bi_OVER,
  bi_DROP,
  bi_ROT,
};

#endif

