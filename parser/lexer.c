/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <string.h>
#include "lexer.h"
#include "token.h"
#include "../util.h"

static pkeyword_t keywords[] = {
  { "if",       2, TOK_KEY_IF       },
  { "in",       2, TOK_KEY_IN       },
  { "is",       2, TOK_KEY_IS       },
  { "or",       2, TOK_KEY_OR       },
  { "and",      3, TOK_KEY_AND      },
  { "for",      3, TOK_KEY_FOR      },
  { "nil",      3, TOK_KEY_NIL      },
  { "var",      3, TOK_KEY_VAR      },
  { "func",     4, TOK_KEY_FUNC     },
  { "else",     4, TOK_KEY_ELSE     },
  { "true",     4, TOK_KEY_TRUE     },
  { "case",     4, TOK_KEY_CASE     },
  { "false",    5, TOK_KEY_FALSE    },
  { "break",    5, TOK_KEY_BREAK    },
  { "while",    5, TOK_KEY_WHILE    },
  { "repeat",   6, TOK_KEY_REPEAT   },
  { "switch",   6, TOK_KEY_SWITCH   },
  { "return",   6, TOK_KEY_RETURN   },
  { "default",  7, TOK_KEY_DEFAULT  },
  { "continue", 8, TOK_KEY_CONTINUE },
  { NULL,       0, TOK_EOF          },
};

plexer_t Lexer;

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

ptoken_t error_token(const char *msg) {
  ptoken_t token;
  token.type = TOK_ERROR;
  token.start = msg;
  token.len = strlen(msg);
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
    if (c == '/' && next() == '/')
      while (peek() != '\n' && !is_at_end()) advance();
    return;
  }
}

static ptoken_t indentifier() {
  while (is_alpha_num(peek())) advance();
  ptoken_type_t type = TOK_IDENTIFIER;

  size_t len = Lexer.current - Lexer.token_start;
  for(pkeyword_t *key = keywords; key->name != NULL; key++){
    if (len == key->len &&
    memcmp(Lexer.token_start, key->name, len)) {
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

static ptoken_t string() {
  while (peek() != '"' && !is_at_end()) {
    if (peek() == '\n') Lexer.line++;
    advance();
  }

  /* unterminated string */
  if (is_at_end()) return error_token("unterminated string");

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


  switch (c) {
    case '(': return make_token(TOK_OP_LEFT_PAREN);
    case ')': return make_token(TOK_OP_RIGHT_PAREN);
    case '[': return make_token(TOK_OP_LEFT_BRACKET);
    case ']': return make_token(TOK_OP_RIGHT_BRACKET);
    case '{': return make_token(TOK_OP_LEFT_BRACE);
    case '}': return make_token(TOK_OP_RIGHT_BRACE);
    case '+':
      if (match('=')) return make_token(TOK_OP_ADD_ASSIGN);
      return make_token(TOK_OP_ADD);
    case '-':
      if (match('=')) return make_token(TOK_OP_SUB_ASSIGN);
      return make_token(TOK_OP_SUB);
    case '/':
      if (match('=')) return make_token(TOK_OP_DIV_ASSIGN);
      return make_token(TOK_OP_DIV);
    case '*':
      if (match('=')) return make_token(TOK_OP_MUL_ASSIGN);
      return make_token(TOK_OP_MUL);
    case '%':
      if (match('=')) return make_token(TOK_OP_MOD_ASSIGN);
      return make_token(TOK_OP_MOD);
    case '=':
      if (match('=')) return make_token(TOK_OP_EQUAL);
      return make_token(TOK_OP_ASSIGN);
    case '<':
      if (match('<'))
        if (match('='))
          return make_token(TOK_OP_SHIFT_LEFT_ASSIGN);
        return make_token(TOK_OP_SHIFT_LEFT);
      if (match('=')) return make_token(TOK_OP_LESS_EQUAL);
      return make_token(TOK_OP_LESS);
    case '>':
      if (match('>'))
        if (match('='))
          return make_token(TOK_OP_SHIFT_RIGHT_ASSIGN);
        return make_token(TOK_OP_SHIFT_RIGHT);
      if (match('=')) return make_token(TOK_OP_GREATER_EQUAL);
      return make_token(TOK_OP_GREATER);
    case '!':
      if (match('=')) return make_token(TOK_OP_NOT_EQUAL);
      return make_token(TOK_OP_NOT);
    case '|':
      if (match('=')) return make_token(TOK_OP_BIT_OR_ASSIGN);
      return make_token(TOK_OP_BIT_OR);
    case '&':
      if (match('=')) return make_token(TOK_OP_BIT_AND_ASSIGN);
      return make_token(TOK_OP_BIT_AND);
    case '^':
      if (match('=')) return make_token(TOK_OP_BIT_XOR_ASSIGN);
      return make_token(TOK_OP_BIT_XOR);
    case '~':
      return make_token(TOK_OP_BIT_NOT);
    // case '?': return make_token(TOK_KEY_NIL);
    case ';': return make_token(TOK_OP_SEMICOLON);
    case ':': return make_token(TOK_OP_COLON);
    case ',': return make_token(TOK_OP_COMMA);
    case '.':
      if (match('.'))
        if (match('.'))
          return make_token(TOK_OP_RANGE_EXCLUDED);
        return make_token(TOK_OP_RANGE_INCLUDED);
      return make_token(TOK_OP_DOT);
    case '"': return string();
  }
  return error_token("unexpected character");
}
