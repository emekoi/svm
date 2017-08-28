/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef OP_H
#define OP_H

#include "svm.h"

enum op_code_values {
	/* early opcodes */
	EXIT = 0x00,

	INT_STORE,
	INT_PRINT,
	INT_TOSTRING,
	INT_RANDOM,

	/* jump operations */
	JUMP_TO = 0x10,
	JUMP_Z,
	JUMP_NZ,

	/* math operations */
	MATH_XOR = 0x20,
  MATH_ADD,
  MATH_SUB,
  MATH_MUL,
  MATH_DIV,
  MATH_INC,
  MATH_DEC,
  MATH_AND,
  MATH_OR,

  /* tring operations */
  STRING_STORE = 0x30,
  STRING_PRINT,
  STRING_CONCAT,
  STRING_SYSTEM,
  STRING_TOINT,

  /* comparison/test operations */
  CMP_REG = 0x40,
  CMP_IMMEDIATE,
  CMP_STRING,
  IS_STRING,
  IS_NUMBER,

  /* misc */
  NOP = 0x50,
  STORE_REG,

  /* peek/poke operations */
  PEEK = 0x60,
  POKE,
  MEMCPY,

  /* stack operations */
  STACK_PUSH = 0x70,
  STACK_POP,
  STACK_RET,
  STACK_CALL
};

/* 0x00 - 0x0F */
void op_exit(svm_t *in);
void op_int_store(svm_t *in);
void op_int_print(svm_t *in);
void op_int_tostring(svm_t *in);
void op_int_random(svm_t *in);

/* 0x10 - 0x1F */
void op_jump_to(svm_t *in);
void op_jump_z(svm_t *in);
void op_jump_nz(svm_t *in);

/* 0x20 - 0x2F */
void op_math_xor(svm_t *in);
void op_math_or(svm_t *in);
void op_math_add(svm_t *in);
void op_math_and(svm_t *in);
void op_math_sub(svm_t *in);
void op_math_mul(svm_t *in);
void op_math_div(svm_t *in);
void op_math_inc(svm_t *in);
void op_math_dec(svm_t *in);

/* 0x30 - 0x3F */
void op_string_store(svm_t *in);
void op_string_print(svm_t *in);
void op_string_concat(svm_t *in);
void op_string_system(svm_t *in);
void op_string_toint(svm_t *in);

/* 0x40 - 0x4F */
void op_cmp_reg(svm_t *in);
void op_cmp_immediate(svm_t *in);
void op_cmp_string(svm_t *in);
void op_is_string(svm_t *in);
void op_is_integer(svm_t *in);

/* 0x50 - 0x5F */
void op_nop(svm_t *in);
void op_reg_store(svm_t *in);


/* 0x60 - 0x6F */
void op_peek(svm_t *in);
void op_poke(svm_t *in);
void op_memcpy(svm_t *in);


/* 0x70 - 0x7F */
void op_stack_push(svm_t *in);
void op_stack_pop(svm_t *in);
void op_stack_ret(svm_t *in);
void op_stack_call(svm_t *in);



/* initialization function */
void op_code_init(svm_t *cpu);

#endif
