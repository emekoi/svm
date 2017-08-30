/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef SVM_H
#define SVM_H

#include <stdlib.h>

#define REGISTER_COUNT 16

typedef struct svm_t svm_t;
typedef struct reg_t reg_t;
typedef struct flag_t flag_t;

typedef void (*op_code_t)(svm_t *vm);

struct flag_t {
	unsigned int z;
};

struct reg_t {
  union {
    unsigned int number;
    char *string;
  } value;
  char *str;
  enum { NUMBER, STRING } type;
};

struct svm_t {
  reg_t registers[REGISTER_COUNT];
  flag_t flags;
  
  unsigned int ip;

  unsigned char *code;
  unsigned int size;

  void (*panic)(char *msg);
  int running;
  
  op_code_t op_codes[256];
  int stack[1024]; int sp;
};

svm_t *svm_new(unsigned char *code, unsigned int size);
void svm_run_n_max(svm_t * cpu, int max);
void svm_run(svm_t *cpu);
void svm_free(svm_t *cpu);

void svm_panic(svm_t * cpu, char *msg);
void svm_panic_set(svm_t *cpu, void (*panic)(char *msg));
void svm_reg_dump(svm_t * cpu);



#endif