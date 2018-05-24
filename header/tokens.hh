#ifndef __TOKENS_HH__
#define __TOKENS_HH__

enum class TOKEN{
  STORE,
  FETCH,
  DEFINE,
  WORD,
  NUMBER,
  DECLARE,
  CONSTANT,
  BRANCH,
  LOOP,
  E_OF_F,
};

enum u_CODE{
  u_STORE,
  u_ADD,
  u_SUB,
  u_AND,
  u_OR,
  u_XOR,
  u_PUSH_CALL,
  u_FETCH,
  u_DROP,
  u_DUP,
  u_OVER,
  u_POP_CALL,
  u_SWAP,
  u_BRANCH,
  u_BRANCH_Z,
  u_BRANCH_R,
  u_BRANCH_R_Z,
  u_CALL,
  u_EXIT,
  u_LIT,
  u_IO,
  u_HALT,
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

