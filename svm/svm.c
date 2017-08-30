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
	if (cpu && cpu->panic) {
		(*cpu->panic)(msg);
		return;
	}
	fprintf(stderr, "\x1b[31mpanic\x1b[0m: %s\n", msg); exit(1);
}


char *escape(char* buffer) {
  int i,j;
  int l = strlen(buffer) + 1;
  char esc_char[] = { '\a','\b','\f','\n','\r','\t','\v','\\' };
  char essc_str[] = {  'a', 'b', 'f', 'n', 'r', 't', 'v','\\' };
  char *dest  =  (char*)calloc(l * 2, sizeof(char));
  char *ptr = dest;
  for (i = 0; i < l; i++) {
    for (j = 0; j < 8; j++) {
      if ( buffer[i]==esc_char[j] ) {
        *ptr++ = '\\';
        *ptr++ = essc_str[j];
        break;
      }
    }
    if (j == 8) *ptr++ = buffer[i];
  }
  *ptr='\0';
  return dest;
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


void svm_reg_dump(svm_t * cpu) {
	printf("register dump\n");

	for (int i = 0; i < REGISTER_COUNT; i++) {
	  switch(cpu->registers[i].type) {
	  	case STRING: {
	  		char *str = escape(cpu->registers[i].value.string);
	  		printf("\tregister %02d - string: \"%s\"\n", i, str);
	  		free(str);
	  		break;
	  	}

	  	case NUMBER: {
	  		int num = cpu->registers[i].value.number;
	  		printf("\tregister %02d - decimal: %04d [hex:%04x]\n", i, num, num);
	  		break;
	  	}
	  	default:
	  		printf("\tregister %02d - unknown\n", i);
	  		break; 
	  }
	}

	if (cpu->flags.z) printf("\tz-flag: true\n");
	else printf("\tz=z-flag: false\n");
}


void svm_free(svm_t *cpu) {
	if (!cpu) return;
	if (cpu->code) {
		free(cpu->code);
		cpu->code = NULL;
	}
  free(cpu);
}


void svm_run(svm_t *cpu) {
	svm_run_n_max(cpu, 0);
}


void svm_run_n_max(svm_t *cpu, int max) {
	if (!cpu) return;
	cpu->ip = 0; int count = 0;

	for (int iterations = 0; cpu->running; iterations++) {
		if (cpu->ip >= 0xffff) cpu->ip = 0;
		int opcode = cpu->code[cpu->ip];

		if (getenv("DEBUG") != NULL) {
			printf("%04x - parsing op_code hex:%02X\n", cpu->ip, opcode);
		}

		if (cpu->op_codes[opcode] != NULL) cpu->op_codes[opcode](cpu);
		iterations++;

		if(max && iterations >= max) {
			cpu->running = 0;
			count = iterations;
		}
        count++;
	}

	if (getenv("DEBUG") != NULL) {
			printf("executed %u instructions\n", count);
	}
}
