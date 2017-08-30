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

/* ======
   PREFIX
   ======
   -       unary minus

   !       logical not

   ~       bitwise not

   =====
   INFIX
   =====
   +      add
   -      subtract
   *      multiply
   /      divide
   %      modulus

   &&     logical and
   ||     logical or
   <      greater than
   <=     greater than or equal
   >      less than
   >=     less than or equal
   ==     equal
   !=     not equal

   >>     bitwise left shift
   <<     bitwise right shift
   |      bitwise or
   &      bitwise and
   ^      bitwise xor

   ?:     ternary
   is     type check
   ..     inclusive range
   ...    exclusive range
   =      assignment

   +=      add and assign
   -=      subtract and assign
   *=      multiply and assign
   /=      divide and assign
   %=      modulus and assign
   >>=     bitwise left shift and assign
   <<=     bitwise right shift and assign
   |=      bitwise or and assign
   &=      bitwise and and assign
   ^=      bitwise xor and assign
*/

typedef enum {
  /* general */
  TOK_EOF = 0, TOK_ERROR, TOK_COMMENT, TOK_STRING, TOK_NUMBER, TOK_IDENTIFIER,

  /* keywords */
  TOK_KEY_FUNC, TOK_KEY_VAR, TOK_KEY_IS, TOK_KEY_RETURN, TOK_KEY_TRUE, TOK_KEY_FALSE,
  TOK_KEY_IF, TOK_KEY_ELSE, TOK_KEY_WHILE, TOK_KEY_FOR, TOK_KEY_IN, TOK_KEY_SWITCH,
  TOK_KEY_DEFAULT, TOK_KEY_CASE, TOK_KEY_CONTINUE, TOK_KEY_BREAK, TOK_KEY_NIL,
  TOK_KEY_REPEAT, TOK_KEY_AND, TOK_KEY_OR,

  /* maybe... */
  // TOK_KEY_DO, TOK_KEY_END,

  /* operators */
  TOK_OP_ADD, TOK_OP_SUB, TOK_OP_MUL, TOK_OP_DIV, TOK_OP_MOD,
  TOK_OP_NOT, TOK_OP_GREATER, TOK_OP_GREATER_EQUAL,
  TOK_OP_LESS, TOK_OP_LESS_EQUAL, TOK_OP_EQUAL, TOK_OP_NOT_EQUAL,

  TOK_OP_SHIFT_LEFT, TOK_OP_SHIFT_RIGHT, TOK_OP_BIT_OR, TOK_OP_BIT_AND, TOK_OP_BIT_XOR, TOK_OP_BIT_NOT,

  TOK_OP_RANGE_INCLUDED, TOK_OP_RANGE_EXCLUDED, TOK_OP_ASSIGN,

  TOK_OP_ADD_ASSIGN, TOK_OP_SUB_ASSIGN, TOK_OP_MUL_ASSIGN, TOK_OP_DIV_ASSIGN,
  TOK_OP_MOD_ASSIGN, TOK_OP_SHIFT_LEFT_ASSIGN, TOK_OP_SHIFT_RIGHT_ASSIGN,
  TOK_OP_BIT_OR_ASSIGN, TOK_OP_BIT_AND_ASSIGN, TOK_OP_BIT_XOR_ASSIGN,

  /* punctuation */
  TOK_OP_LEFT_PAREN, TOK_OP_RIGHT_PAREN, TOK_OP_LEFT_BRACKET, TOK_OP_RIGHT_BRACKET,
  TOK_OP_LEFT_BRACE, TOK_OP_RIGHT_BRACE, TOK_OP_SEMICOLON, TOK_OP_COMMA, TOK_OP_DOT,
  TOK_OP_COLON,

  /* end of tokens */
  TOK_END
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
  unsigned hash;
} pkeyword_t;

const char *token_name(ptoken_type_t token);
ptoken_type_t token_keyword(const char *buffer, size_t len);

#endif
