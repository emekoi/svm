/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  /* general */
  TOK_EOF = -0x0F,
  TOK_ERROR,
  TOK_COMMENT,
  TOK_STRING,
  TOK_NUMBER,
  TOK_LABEL,
  TOK_END,

  TOK_OP_EXIT = 0x00,


  /* int operation */
  TOP_OP_INT_STORE,
  TOK_OP_INT_PRINT,
  TOK_OP_INT_TOSTRING,
  TOK_OP_INT_RANDOM,

  /* jump operations */
  TOK_OP_JUMP_TO = 0x10,
  TOK_OP_JUMP_Z,
  TOK_OP_JUMP_NZ,

  /* math operations */
  TOK_OP_MATH_XOR = 0x20,
  TOK_OP_MATH_ADD,
  TOK_OP_MATH_SUB,
  TOK_OP_MATH_MUL,
  TOK_OP_MATH_DIV,
  TOK_OP_MATH_INC,
  TOK_OP_MATH_DEC,
  TOK_OP_MATH_AND,
  TOK_OP_MATH_LFT,
  TOK_OP_MATH_RGT,
  TOK_OP_MATH_NOT,
  TOK_OP_MATH_OR,

  /* string operations */
  TOK_OP_STRING_STORE = 0x30,
  TOK_OP_STRING_PRINT,
  TOK_OP_STRING_CONCAT,
  TOK_OP_STRING_SYSTEM,
  TOK_OP_STRING_TOINT,

  /* comparison/test operations */
  TOK_OP_CMP_REG = 0x40,
  TOK_OP_CMP_IMMEDIATE,
  TOK_OP_CMP_STRING,
  TOK_OP_IS_STRING,
  TOK_OP_IS_NUMBER,

  /* misc */
  TOK_OP_NOP = 0x50,
  TOK_OP_STORE_REG,

  /* peek/poke operations */
  TOK_OP_PEEK = 0x60,
  TOK_OP_POKE,
  TOK_OP_MEMCPY,

  /* stack operations */
  TOK_OP_STACK_PUSH = 0x70,
  TOK_OP_STACK_POP,
  TOK_OP_STACK_RET,
  TOK_OP_STACK_CALL,

  /* misc. */
  TOK_COMMA,
  TOK_COLON,
  TOK_SEMICOLON,
} ptoken_type_t;

typedef enum {
  LITERAL_STRING, LITERAL_FLOAT, LITERAL_INT, LITERAL_BOOL, LITERAL_INTERPOLATED
} pliteral_type_t;

typedef struct {
  ptoken_type_t type;
  const char *start;
  size_t len;
  size_t line;
} ptoken_t;

typedef struct {
  const char* name;
  size_t len;
  ptoken_type_t type;
} pkeyword_t;

const char *token_name(ptoken_type_t token);
ptoken_type_t token_keyword(const char *buffer, size_t len);

#endif
