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
    case TOK_LABEL: return "LABEL";
    case TOK_REGISTER: return "REGISTER";


    case TOK_OP_EXIT: return "EXIT";

    case TOP_OP_INT_STORE: return "INT_STORE";
    case TOK_OP_INT_PRINT: return "INT_PRINT";
    case TOK_OP_INT_TOSTRING: return "INT_TOSTRING";
    case TOK_OP_INT_RANDOM: return "INT_RANDOM";

    case TOK_OP_JUMP_TO: return "JUMP_TO";
    case TOK_OP_JUMP_Z: return "JUMP_Z";
    case TOK_OP_JUMP_NZ: return "JUMP_NZ";

    case TOK_OP_MATH_XOR: return "MATH_XOR";
    case TOK_OP_MATH_ADD: return "MATH_ADD";
    case TOK_OP_MATH_SUB: return "MATH_SUB";
    case TOK_OP_MATH_MUL: return "MATH_MUL";
    case TOK_OP_MATH_DIV: return "MATH_DIV";
    case TOK_OP_MATH_INC: return "MATH_INC";
    case TOK_OP_MATH_DEC: return "MATH_DEC";
    case TOK_OP_MATH_AND: return "MATH_AND";
    case TOK_OP_MATH_LFT: return "MATH_LFT";
    case TOK_OP_MATH_RGT: return "MATH_RGT";
    case TOK_OP_MATH_NOT: return "MATH_NOT";
    case TOK_OP_MATH_OR: return "MATH_OR";

    case TOK_OP_STRING_STORE: return "STRING_STORE";
    case TOK_OP_STRING_PRINT: return "STRING_PRINT";
    case TOK_OP_STRING_CONCAT: return "STRING_CONCAT";
    case TOK_OP_STRING_SYSTEM: return "STRING_SYSTEM";
    case TOK_OP_STRING_TOINT: return "STRING_TOINT";

    case TOK_OP_CMP_REG: return "CMP_REG";
    case TOK_OP_CMP_IMMEDIATE: return "CMP_IMMEDIATE";
    case TOK_OP_CMP_STRING: return "CMP_STRING";
    case TOK_OP_IS_STRING: return "IS_STRING";
    case TOK_OP_IS_NUMBER: return "IS_NUMBER";

    case TOK_OP_NOP: return "NOP";
    case TOK_OP_STORE_REG: return "STORE_REG";

    case TOK_OP_PEEK: return "PEEK";
    case TOK_OP_POKE: return "POKE";
    case TOK_OP_MEMCPY: return "MEMCPY";

    case TOK_OP_STACK_PUSH: return "STACK_PUSH";
    case TOK_OP_STACK_POP: return "STACK_POP";
    case TOK_OP_STACK_RET: return "STACK_RET";
    case TOK_OP_STACK_CALL: return "STACK_CALL";

    case TOK_COLON: return ":";
    case TOK_COMMA: return ",";
    case TOK_END: return "END";
  }

  /* should never reach this point */
  return "???";
}

// ptoken_type_t token_keyword(const char *buffer, size_t len) {
//   switch (len) {
//     case 2:
//       if (string_casencmp(buffer, "if", len) == 0) return TOK_KEY_IF;
//       if (string_casencmp(buffer, "in", len) == 0) return TOK_KEY_IN;
//       if (string_casencmp(buffer, "is", len) == 0) return TOK_KEY_IS;
//       break;
//
//     case 3:
//       if (string_casencmp(buffer, "for", len) == 0) return TOK_KEY_FOR;
//       if (string_casencmp(buffer, "var", len) == 0) return TOK_KEY_VAR;
//       if (string_casencmp(buffer, "nil", len) == 0) return TOK_KEY_NIL;
//       break;
//
//     case 4:
//       if (string_casencmp(buffer, "func", len) == 0) return TOK_KEY_FUNC;
//       if (string_casencmp(buffer, "else", len) == 0) return TOK_KEY_ELSE;
//       if (string_casencmp(buffer, "true", len) == 0) return TOK_KEY_TRUE;
//       if (string_casencmp(buffer, "case", len) == 0) return TOK_KEY_CASE;
//       break;
//
//     case 5:
//       if (string_casencmp(buffer, "false", len) == 0) return TOK_KEY_FALSE;
//       if (string_casencmp(buffer, "break", len) == 0) return TOK_KEY_BREAK;
//       if (string_casencmp(buffer, "while", len) == 0) return TOK_KEY_WHILE;
//       break;
//
//     case 6:
//       if (string_casencmp(buffer, "repeat", len) == 0) return TOK_KEY_REPEAT;
//       if (string_casencmp(buffer, "switch", len) == 0) return TOK_KEY_SWITCH;
//       if (string_casencmp(buffer, "return", len) == 0) return TOK_KEY_RETURN;
//       break;
//
//     case 7:
//       if (string_casencmp(buffer, "default", len) == 0) return TOK_KEY_DEFAULT;
//       break;
//
//     case 8:
//       if (string_casencmp(buffer, "continue", len) == 0) return TOK_KEY_CONTINUE;
//       break;
//   }
//
//   return TOK_IDENTIFIER;
// }
