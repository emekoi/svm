/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "token.h"
#include "../util.h"

/*

:test
:label
[-] goto  0x44ff      # Jump to the given address
[-] goto  label       # Jump to the given label
[-] jmpnz label       # Jump to label if Zero-Flag is not set
[-] jmpz  label       # Jump to label if Zero-Flag is set

[] store #1, 33      # store 33 in register 1
[] store #2, "Steve" # Store the string "Steve" in register 1.
[] store #1, #3      # register1 is set to the contents of register #3.

[-] exit              # Stop processing.
[-] nop               # Do nothing

[-] print_int #3      # Print the (integer) contents of register 3
[-] print_str #3      # Print the (string) contents of register 3

[-] system #3         # Call the (string) command stored in register 3

[-] add #1, #2, #3    # Add register 2 + register 3 contents, store in reg 1
[-] sub #1, #2, #3    # sub register 2 + register 3 contents, store in reg 1
[-] mul #1, #2, #3    # multiply register 2 + register 3 contents, store in reg 1
[-] concat #1, #2,#3  # store concatenated strings from reg2 + reg3 in reg1.

[-] dec #2            # Decrement the integer in register 2
[-] inc #2            # Increment the integer in register 2

[-] string2int #3     # Change register 3 to have a string from an int
[-] is_integer #3     # Does the given register have an integer content?
[-] int2string #3     # Change register 3 to have an int from a string
[-] is_string  #3     # Does the given register have a string-content?

[] cmp #3, #4        # Compare contents of reg 3 & 4, set the Z-flag.
[] cmp #3, 42        # Compare contents of reg 3 with the constant 42.  sets z.
[] cmp #3, "Moi"     # Compare contents of reg 3 with the constant string "Moi".  sets z.

[-] peek #1, #4       # Load register 1 with the contents of the address in #4.
[-] poke #1, #4       # Set the address stored in register4 with the contents of reg1.
[-] random #2         # Store a random integer in register #2.

[-] push #1           # Store the contents of register #1 in the stack
[-] pop  #1           # Load register #1 with the contents of the stack.
[-] call 0xFFEE       # Call the given address.
[-] call my_label     # Call the defined label
[-] ret               # Return from a called-routine.

*/

static pkeyword_t keywords[] = {
  { "nop", 3, TOK_OP_NOP },
  { "exit", 4, TOK_OP_EXIT },
  { "peek", 4, TOK_OP_PEEK },
  { "poke", 4, TOK_OP_POKE },
  { "memcpy", 5, TOK_OP_MEMCPY },

  { "goto", 4, TOK_OP_JUMP_TO },
  { "jmpz", 4, TOK_OP_JUMP_Z },
  { "jmpnz", 5, TOK_OP_JUMP_NZ },

  { "pop", 3, TOK_OP_STACK_POP },
  { "ret", 3, TOK_OP_STACK_RET },
  { "call", 4, TOK_OP_STACK_CALL },
  { "push", 4, TOK_OP_STACK_PUSH },

  { "or",  2, TOK_OP_MATH_OR },
  { "xor", 3, TOK_OP_MATH_XOR },
  { "add", 3, TOK_OP_MATH_ADD },
  { "sub", 3, TOK_OP_MATH_SUB },
  { "mul", 3, TOK_OP_MATH_MUL },
  { "div", 3, TOK_OP_MATH_DIV },
  { "inc", 3, TOK_OP_MATH_INC },
  { "dec", 3, TOK_OP_MATH_DEC },
  { "and", 3, TOK_OP_MATH_AND },
  { "lft", 3, TOK_OP_MATH_LFT },
  { "rgt", 3, TOK_OP_MATH_RGT },
  { "not", 3, TOK_OP_MATH_NOT },

  { "store", 5, TOP_OP_INT_STORE },
  { "random", 6, TOK_OP_INT_RANDOM },
  { "print_int", 9, TOK_OP_INT_PRINT },
  { "int2string", 10, TOK_OP_INT_TOSTRING },

  { "store", 5, TOK_OP_STRING_STORE },
  { "concat", 6, TOK_OP_STRING_CONCAT },
  { "system", 6, TOK_OP_STRING_SYSTEM },
  { "print_str", 9, TOK_OP_STRING_PRINT },
  { "string2int", 10, TOK_OP_STRING_TOINT },

  { "cmp", 3, TOK_OP_CMP_REG },
  { "cmp", 3, TOK_OP_CMP_IMMEDIATE },
  { "cmp", 3, TOK_OP_CMP_STRING },
  { "is_string", 9, TOK_OP_IS_STRING },
  { "is_integer", 10, TOK_OP_IS_NUMBER },

  { "store", 5, TOK_OP_STORE_REG },

  { NULL,       0, TOK_EOF,          },
};

plexer_t Lexer;

static unsigned hash(const char *name) {
  unsigned hash = 0;
  const char *p = name;
  while (*p) hash ^= (hash << 5) + (hash >> 2) + *p++;
  return hash;
}

void lexer_init(const char *source) {
  Lexer.source = source;
  Lexer.token_start = source;
  Lexer.current = source;
  Lexer.line = 0;
}

static bool is_alpha(char c) {
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         (c == '_');
}

static bool is_digit(char c) {
  return (c >= '0' && c <= '9');
}

static bool is_alpha_num(char c) {
  return is_alpha(c) || is_digit(c);
}

static bool is_at_end() {
  return *Lexer.current == '\0';
}

static char advance() {
  Lexer.current++;
  return Lexer.current[-1];
}

static char peek() {
  return *Lexer.current;
}

static char next() {
  if (is_at_end()) return '\0';
  return Lexer.current[1];
}

static bool match(char e) {
  if (is_at_end()) return false;
  if (*Lexer.current != e) return false;
  Lexer.current++;
  return true;
}

ptoken_t make_token(ptoken_type_t t) {
  ptoken_t token;
  token.type = t;
  token.start = Lexer.token_start;
  token.len = (size_t)(Lexer.current - Lexer.token_start);
  token.line = Lexer.line;
  return token;
}

// ptoken_t error_token(const char *msg) {
//   ptoken_t token;
//   token.type = TOK_ERROR;
//   token.start = msg;
//   token.len = strlen(msg);
//   token.line = Lexer.line;
//   return token;
// }

ptoken_t error_token(const char c) {
  ptoken_t token;
  token.type = TOK_ERROR;
  token.start = &c;
  token.len = 1;
  token.line = Lexer.line;
  return token;
}

#define WHITESPACE  " \n\t\r"

void skip_whitespace() {
  for (;;) {
    char c = peek();

    while (c && strchr(WHITESPACE, c)) {
      if (c == '\n') Lexer.line++;
      advance();
      c = peek();
    }

    if (match('#') && !is_digit(peek())) {
      while (peek() != '\n' && !is_at_end()) putchar(advance());
      if (peek() == '\n') {
        advance();
        Lexer.line++;
        putchar('\n');
      }
    }

    return;
  }
}

static ptoken_t indentifier() {
  while (is_alpha_num(peek())) advance();
  ptoken_type_t type = TOK_COMMENT;

  size_t len = Lexer.current - Lexer.token_start;
  for(pkeyword_t *key = keywords; key->name != NULL; key++){
    if (len == key->len && !memcmp(Lexer.token_start, key->name, len)) {
      type = key->type;
      break;
    }
  }
  return make_token(type);
}

static ptoken_t number() {
  while (is_digit(peek())) advance();

  /* look for fractional part */
  if (peek() == '.' && is_digit(next())) {
    /* consume decimal */
    advance();
    while (is_digit(peek())) advance();
  }

  return make_token(TOK_NUMBER);
}

static ptoken_t _register() {
  if (match('#'))
    while (is_digit(peek())) advance();
  return make_token(TOK_REGISTER);
}

static ptoken_t label() {
  if (match(':'))
    // if ()
    while (is_digit(peek())) advance();
  return make_token(TOK_REGISTER);
}

static ptoken_t string() {
  while (peek() != '"' && !is_at_end()) {
    if (peek() == '\n') Lexer.line++;
    advance();
  }

  /* unterminated string */
  // if (is_at_end()) return error_token("unterminated string");

  /* the closing '"' */
  advance();
  return make_token(TOK_STRING);
}

ptoken_t lexer_get_token() {
  skip_whitespace();

  /* next token starts with current character */
  Lexer.token_start = Lexer.current;

  if (is_at_end()) return make_token(TOK_EOF);

  char c = advance();

  if (is_alpha(c)) return indentifier();
  if (is_digit(c)) return number();
  // printf("%c", c);

  switch (c) {
    // case '(': return make_token(TOK_OP_LEFT_PAREN);
    // case '+':
    //   if (match('=')) return make_token(TOK_OP_ADD_ASSIGN);
    //   return make_token(TOK_OP_ADD);
    // case '=':
    //   if (match('=')) return make_token(TOK_OP_EQUAL);
    //   return make_token(TOK_OP_ASSIGN);
    // case '<':
    //   if (match('<'))
    //     if (match('='))
    //       return make_token(TOK_OP_SHIFT_LEFT_ASSIGN);
    //     return make_token(TOK_OP_SHIFT_LEFT);
    //   if (match('=')) return make_token(TOK_OP_LESS_EQUAL);
    //   return make_token(TOK_OP_LESS);
    // case '!':
    //   if (match('=')) return make_token(TOK_OP_NOT_EQUAL);
    //   return make_token(TOK_OP_NOT);
    // case '|':
    //   if (match('=')) return make_token(TOK_OP_BIT_OR_ASSIGN);
    //   return make_token(TOK_OP_BIT_OR);
    // case '&':
    //   if (match('=')) return make_token(TOK_OP_BIT_AND_ASSIGN);
    //   return make_token(TOK_OP_BIT_AND);
    // case '^':
    //   if (match('=')) return make_token(TOK_OP_BIT_XOR_ASSIGN);
    //   return make_token(TOK_OP_BIT_XOR);
    case '#': return _register();
    case ':': return label();
    case ',': return make_token(TOK_COMMA);
    case '"': return string();
  }
  return error_token(c);
  // return error_token("unexpected character");
}
