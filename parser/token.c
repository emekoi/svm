/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <string.h>
#include "token.h"
#include "../util.h"


const char *token_name(ptoken_type_t token) {
  switch (token) {
    case TOK_EOF: return "EOF";
    case TOK_ERROR: return "ERROR";
    case TOK_COMMENT: return "COMMENT";
    case TOK_STRING: return "STRING";
    case TOK_NUMBER: return "NUMBER";
    case TOK_IDENTIFIER: return "IDENTIFIER";

    /* keywords */
    case TOK_KEY_FUNC: return "func";
    case TOK_KEY_DEFAULT: return "default";
    case TOK_KEY_TRUE: return "true";
    case TOK_KEY_FALSE: return "false";
    case TOK_KEY_IF: return "if";
    case TOK_KEY_ELSE: return "else";
    case TOK_KEY_SWITCH: return "switch";
    case TOK_KEY_BREAK: return "break";
    case TOK_KEY_CONTINUE: return "continue";
    case TOK_KEY_RETURN: return "return";
    case TOK_KEY_WHILE: return "while";
    case TOK_KEY_REPEAT: return "repeat";
    case TOK_KEY_FOR: return "for";
    case TOK_KEY_IN: return "in";
    case TOK_KEY_VAR: return "var";
    case TOK_KEY_CASE: return "case";
    case TOK_KEY_NIL: return "nil";
    case TOK_KEY_IS: return "is";
    case TOK_KEY_AND: return "and";
    case TOK_KEY_OR: return "or";

    /* operators */
    case TOK_OP_ADD: return "+";
    case TOK_OP_SUB: return "-";
    case TOK_OP_DIV: return "/";
    case TOK_OP_MUL: return "*";
    case TOK_OP_MOD: return "%";
    case TOK_OP_ASSIGN: return "=";
    case TOK_OP_LESS: return "<";
    case TOK_OP_GREATER: return ">";
    case TOK_OP_LESS_EQUAL: return "<=";
    case TOK_OP_GREATER_EQUAL: return ">=";
    case TOK_OP_ADD_ASSIGN: return "+=";
    case TOK_OP_SUB_ASSIGN: return "-=";
    case TOK_OP_DIV_ASSIGN: return "/=";
    case TOK_OP_MUL_ASSIGN: return "*=";
    case TOK_OP_MOD_ASSIGN: return "%=";
    case TOK_OP_NOT: return "!";
    case TOK_OP_EQUAL: return "==";
    case TOK_OP_NOT_EQUAL: return "!=";
    case TOK_OP_RANGE_INCLUDED: return "..";
    case TOK_OP_RANGE_EXCLUDED: return "...";
    case TOK_OP_SHIFT_LEFT: return "<<";
    case TOK_OP_SHIFT_RIGHT: return ">>";
    case TOK_OP_BIT_AND: return "&";
    case TOK_OP_BIT_OR: return "|";
    case TOK_OP_BIT_XOR: return "^";
    case TOK_OP_BIT_NOT: return "~";
    case TOK_OP_SHIFT_LEFT_ASSIGN: return "<<=";
    case TOK_OP_SHIFT_RIGHT_ASSIGN: return ">>=";
    case TOK_OP_BIT_AND_ASSIGN: return "&=";
    case TOK_OP_BIT_OR_ASSIGN: return "|=";
    case TOK_OP_BIT_XOR_ASSIGN: return "^=";

    case TOK_OP_LEFT_PAREN: return "(";
    case TOK_OP_RIGHT_PAREN: return ")";
    case TOK_OP_LEFT_BRACKET: return "[";
    case TOK_OP_RIGHT_BRACKET: return "]";
    case TOK_OP_LEFT_BRACE: return "{";
    case TOK_OP_RIGHT_BRACE: return "}";
    case TOK_OP_SEMICOLON: return ";";
    case TOK_OP_COLON: return ":";
    case TOK_OP_COMMA: return ",";
    case TOK_OP_DOT: return ".";

    case TOK_END: return "";
  }

  /* should never reach this point */
  return "???";
}

ptoken_type_t token_keyword(const char *buffer, size_t len) {
  switch (len) {
    case 2:
      if (string_casencmp(buffer, "if", len) == 0) return TOK_KEY_IF;
      if (string_casencmp(buffer, "in", len) == 0) return TOK_KEY_IN;
      if (string_casencmp(buffer, "is", len) == 0) return TOK_KEY_IS;
      break;

    case 3:
      if (string_casencmp(buffer, "for", len) == 0) return TOK_KEY_FOR;
      if (string_casencmp(buffer, "var", len) == 0) return TOK_KEY_VAR;
      if (string_casencmp(buffer, "nil", len) == 0) return TOK_KEY_NIL;
      break;

    case 4:
      if (string_casencmp(buffer, "func", len) == 0) return TOK_KEY_FUNC;
      if (string_casencmp(buffer, "else", len) == 0) return TOK_KEY_ELSE;
      if (string_casencmp(buffer, "true", len) == 0) return TOK_KEY_TRUE;
      if (string_casencmp(buffer, "case", len) == 0) return TOK_KEY_CASE;
      break;

    case 5:
      if (string_casencmp(buffer, "false", len) == 0) return TOK_KEY_FALSE;
      if (string_casencmp(buffer, "break", len) == 0) return TOK_KEY_BREAK;
      if (string_casencmp(buffer, "while", len) == 0) return TOK_KEY_WHILE;
      break;

    case 6:
      if (string_casencmp(buffer, "repeat", len) == 0) return TOK_KEY_REPEAT;
      if (string_casencmp(buffer, "switch", len) == 0) return TOK_KEY_SWITCH;
      if (string_casencmp(buffer, "return", len) == 0) return TOK_KEY_RETURN;
      break;

    case 7:
      if (string_casencmp(buffer, "default", len) == 0) return TOK_KEY_DEFAULT;
      break;

    case 8:
      if (string_casencmp(buffer, "continue", len) == 0) return TOK_KEY_CONTINUE;
      break;
  }

  return TOK_IDENTIFIER;
}