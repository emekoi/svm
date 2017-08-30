/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include "parser/lexer.h"
#include "parser/token.h"
#include "svm/svm.h"
#include "svm/op.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

// https://cturt.github.io/cinoop.html
// https://github.com/skx/simple.vm
// https://felixangell.com/blog/virtual-machine-in-c
// https://rsms.me/sol-a-sunny-little-virtual-machine

// svm_t *VM;

void shutdown() {
  // svm_free(VM);
}

int run_file(char *filename, int dump_reg, int instr_max) {
  struct stat sb;
  if (stat(filename, &sb) != 0) {
    printf("failed to read file: %s\n", filename);
    return 1;
  }

  int size = sb.st_size;

  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    printf("failed to open file: %s\n", filename);
    return 1;
  }

  unsigned char *code = malloc(size);
  memset(code, '\0', size);

  /* parse input `experimental` */
  // char *code = calloc(1, size);
  // memset(code, '\0', size);
  

  if (!code) {
    printf("failed to allocate ram for file: %s\n", filename);
    fclose(fp); return 1;
  }

  /* abort on a short-read, or error */
  size_t read = fread(code, 1, size, fp);
  if (read < 1 || (read < (size_t)size)) {
    printf("failed to completely read file: %s\n", filename);
    fclose(fp); return 1;
  }

  fclose(fp);

  /* parse input `experimental` */
  // lexer_init(code);
  // ptoken_t t = lexer_get_token();
  // while (t.type != TOK_EOF) {
  //   if (t.type == TOK_ERROR) printf("%s: %lu: ", filename, t.line);
  //   else printf("%s: ", token_name(t.type));
  // } 
  // for (size_t i = 0; i < t.len; i++) putchar(*(t.start+i));
  //   putchar('\n');
  //   t = lexer_get_token();
  // }

  svm_t *cpu = svm_new(code, size);
  if (!cpu) {
    printf("failed to create virtual machine instance for file: %s\n", filename);
    return 1;
  }

  /* run the bytecode */
  svm_run_n_max(cpu, instr_max);

  /* dump the registers? */
  if (dump_reg) svm_reg_dump(cpu);

  /* cleanup */
  svm_free(cpu);
  free(code);
  return 0;
}


/**
 * simple driver for launching virtual machine
 * given a filename parse/execute the opcodes contained within it
**/
int main(int argc, char **argv) {
  int dump_reg = 0;
  int instr_max = 0;

  if (argc < 2) {
    printf("usage: %s input max\n", argv[0]);
    return 0;
  }

  if (argc >= 2) {
    instr_max = (argv[2] ? atoi(argv[2]) : 0);
    if (getenv("DEBUG") != NULL) dump_reg = 1;

    return run_file(argv[1], dump_reg, instr_max);
  }
}