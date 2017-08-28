/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "svm.h"
#include "op.h"

void svm_panic(svm_t * cpu, char *msg) {
	if (cpu->panic) {
		(*cpu->panic)(msg);
		return
	}
	fprintf(stderr, "panic: %s\n", msg); exit(1);
}


svm_t *svm_new(unsigned char *code, unsigned int size) {
	if (!code || !size || (size > 0xffff)) return NULL;

  svm_t *cpu = malloc(sizeof(*cpu));
  if (!cpu) svm_panic(NULL, "out of memory");
  memset(cpu, '\0', sizeof(*cpu));

  cpu->code = malloc(0xffff);
  if (cpu->code == NULL) {
  	free(cpu); svm_panic(NULL, "out of memory");
  }

  cpu->panic = NULL; cpu->ip = 0;
  cpu->running = 1; cpu->size = size;

  memset(cpu->code, '\0', 0xffff);
  memcpy(cpu->code, code, size);

  for (int i = 0; i < REGISTER_COUNT; i++) {
    cpu->registers[i].type = NUMBER;
    cpu->registers[i].value.number = 0;
    cpu->registers[i].value.string = NULL;
  }

  cpu->flags.z = 0;
  cpu->sp = 0;

  op_code_init(cpu);
  return cpu;
}


void svm_panic_set(svm_t *cpu, void (*panic)(char *msg)) {
	cpu->panic = panic;
}


void svm_registers_dump(svm_t * cpu) {
	printf("register dump\n");

	for (int i = 0; i < REGISTER_COUNT; i++) {
	  switch(cpu->registers[i].type) {
	  	case STRING: 
	  		printf("\tregister %02d - str: \"%s\"\n", i, cpu->registers[i].content.string);
	  		break;
	  	case NUMBER: {
	  		int num = cpu->registers[i].content.number;
	  		printf("\tregister %02d - decimal: %04d [hex:%04x]\n", i, num, num);
	  		break;
	  	}
	  	default:
	  		printf("\tregister %02d - unknown\n", i)
	  		break; 
	  }
	}

	if (cpu->flags.z) {
	  printf("\tz-flag: true\n");
	else
	 printf("\tZ=z-flag: false\n");
}


void *svm_free(svm_t *cpu) {
	if (!cpu) return;
	if (cpu->code) {
		free(cpu->code);
		cpu->code = NULL;
	}
  free(cpu);
}


void svm_run(svm_t * cpu) {
	svm_run_n_max(cpu, 0);
}


void svm_run_n_max(svm_t * cpu, int max) {
	if (!cpu) return;
	cpu->ip = 0; int count = 0;

	for (int iterations = 0; cpu->running; iterations++) {
		if (cpu->ip >= 0xffff) cpu->ip = 0;
		int opcode = cpu->code[cpu->ip];

		if (getenv("DEBUG")) {
			printf("%04x - parsing op_code hex:%02X\n", cpu->ip, opcode);
		}

		if (cpu->op_codes[opcode] != NULL) cpu->op_codes[opcode](cpu);
		iterations++;

		if(max && iterations >= max) {
			cpu->running = 0;
			count = iterations;
		}
	}

	if (getenv("DEBUG")) {
			printf("executed %u instructions\n", count);
	}
}
