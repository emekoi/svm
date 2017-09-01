/**
* Copyright (c) 2017 emekoi
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the MIT license. See LICENSE for details.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "op.h"

#define BOUNDS_TEST_REG(reg) if (reg >= REGISTER_COUNT ) svm_panic(svm, "reegister out of bounds");
#define BYTES_TO_ADDR(one,two) (one + ( 256 * two ))

#define MATH_OPERATION(function,operator)  void function(svm_t* svm) { \
  /* get the destination register */ \
  unsigned int reg = next_byte(svm); \
  BOUNDS_TEST_REG(reg); \
  \
  /* get the source register */ \
  unsigned int src1 = next_byte(svm); \
  BOUNDS_TEST_REG(reg); \
  \
  /* get the source register */\
  unsigned int src2 = next_byte(svm);\
  BOUNDS_TEST_REG(reg);\
  \
  if (getenv("DEBUG") != NULL)\
    printf( #function "(register: %d = register:%d " #operator " register: %d)\n", reg, src1, src2); \
  \
  /* if the result-register stores a string .. free it */\
  if ((svm->registers[reg].type == STRING) && (svm->registers[reg].value.string))\
    free(svm->registers[reg].value.string);\
  \
  /* \
  * ensure both source registers have number values.\
  */\
  int val1 = get_int_reg(svm, src1);\
  int val2 = get_int_reg(svm, src2);\
  \
  /** \
  * Store the result.\
  */\
  svm->registers[reg].value.number = val1 operator val2; \
  svm->registers[reg].type = NUMBER; \
  \
  /**\
  * Zero result? \
  */\
  if (svm->registers[reg].value.number == 0)\
    svm->flags.z = 1;\
  else\
    svm->flags.z = 0;\
  \
  /* handle the next instruction */ \
  svm->ip += 1; \
}

char *strdup(const char *src) {
  size_t len = strlen(src) + 1;
  char *s = malloc(len);
  if (s == NULL) return NULL;
  return (char *)memcpy(s, src, len);
}


char *get_string_reg(svm_t* cpu, int reg);
int get_int_reg(svm_t* cpu, int reg);
char *string_from_stack(svm_t* svm);
unsigned char next_byte(svm_t* svm);

char *get_string_reg(svm_t * cpu, int reg) {
  if (cpu->registers[reg].type == STRING)
    return (cpu->registers[reg].value.string);

  svm_panic(cpu, "the register doesn't contain a string");
  return NULL;
}

int get_int_reg(svm_t * cpu, int reg) {
  if (cpu->registers[reg].type == NUMBER)
    return (cpu->registers[reg].value.number);

  svm_panic(cpu, "The register doesn't contain an number");
  return 0;
}

char *string_from_stack(svm_t* svm) {
  /* the string length */
  unsigned int len1 = next_byte(svm);
  unsigned int len2 = next_byte(svm);

  /* build up the length 0-64k */
  int len = BYTES_TO_ADDR(len1, len2);

  /* bump IP one more to point to the start of the string-data. */
  svm->ip += 1;

  /* allocate enough RAM to contain the string. */
  char *tmp = (char *)malloc(len + 1);
  if (tmp == NULL) svm_panic(svm, "RAM allocation failure.");

  /**
  * Zero the allocated memory, and copy the string-values over.
  *
  * The copy is inefficient - but copes with embedded NULL.
  */
  memset(tmp, '\0', len + 1);
  for (int i = 0; i < (int) len; i++) {
    tmp[i] = svm->code[svm->ip];
    svm->ip++;
  }

  svm->ip--;
  return tmp;
}

unsigned char next_byte(svm_t* svm) {
  svm->ip += 1;

  if (svm->ip >= 0xFFFF) svm->ip = 0;

  return (svm->code[svm->ip]);
}


/**
** Start implementation of virtual machine op_codes.
**
**/


void op_unknown(svm_t * svm) {
  int instruction = svm->code[svm->ip];
  printf("%04X - op_unknown(%02X)\n", svm->ip, instruction);

  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Break out of our main intepretter loop.
*/
void op_exit(svm_t *svm) {
  svm->running = 0;

  /* handle the next instruction - which won't happen */
  svm->ip += 1;
}


/**
* No-operation / NOP.
*/
void op_nop(svm_t *svm) {
  (void) svm;

  if (getenv("DEBUG") != NULL) printf("nop()\n");

  /* handle the next instruction */
  svm->ip += 1;
}


void op_math_div(svm_t * svm) {
  /* get the destination register */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  /* get the source register */
  unsigned int src1 = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  /* get the source register */
  unsigned int src2 = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  if (getenv("DEBUG") != NULL)
    printf( "DIV (register:%d = Register:%d / Register:%d)\n", reg, src1, src2);

  /* if the result-register stores a string .. free it */
  if ((svm->registers[reg].type == STRING) && (svm->registers[reg].value.string))
    free(svm->registers[reg].value.string);

  /*
  * Ensure both source registers have number values.
  */
  int val1 = get_int_reg(svm, src1);
  int val2 = get_int_reg(svm, src2);

  if ( val2 == 0 ) {
    svm_panic(svm, "Division by zero!");
    return;
  }

  /**
  * Store the result.
  */
  svm->registers[reg].value.number = val1 / val2;
  svm->registers[reg].type = NUMBER;

  /**
  * Zero result?
  */
  if (svm->registers[reg].value.number == 0) svm->flags.z = 1;
  else svm->flags.z = 0;

  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Store the values of one register in another.
*/
void op_reg_store(svm_t *svm) {
  (void) svm;

  /* get the destination register */
  unsigned int dst = next_byte(svm);
  BOUNDS_TEST_REG(dst);

  /* get the source register */
  unsigned int src = next_byte(svm);
  BOUNDS_TEST_REG(src);

  if (getenv("DEBUG") != NULL)
    printf("STORE (reg%02x will be set to values of Reg%02x)\n", dst, src);

  /* Free the existing string, if present */
  if ((svm->registers[dst].type == STRING) && (svm->registers[dst].value.string))
    free(svm->registers[dst].value.string);


  /* if storing a string - then use strdup */
  if (svm->registers[src].type == STRING) {
    svm->registers[dst].type = STRING;
    svm->registers[dst].value.string = strdup(svm->registers[src].value.string);
  } else
  {
    svm->registers[dst].type = svm->registers[src].type;
    svm->registers[dst].value.number = svm->registers[src].value.number;
  }


  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Store an number in a register.
*/
void op_int_store(svm_t *svm) {
  /* get the register number to store in */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  /* get the value */
  unsigned int val1 = next_byte(svm);
  unsigned int val2 = next_byte(svm);
  int value = BYTES_TO_ADDR(val1, val2);

  if (getenv("DEBUG") != NULL)
    printf("STORE_INT (reg:%02x) => %04d [Hex:%04x]\n", reg, value, value);

  /* if the register stores a string .. free it */
  if ((svm->registers[reg].type == STRING) && (svm->registers[reg].value.string))
    free(svm->registers[reg].value.string);

  svm->registers[reg].value.number = value;
  svm->registers[reg].type = NUMBER;

  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Print the number values of the given register.
*/
void op_int_print(svm_t *svm) {
  /* get the register number to print */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  if (getenv("DEBUG") != NULL)
    printf("INT_PRINT (register %d)\n", reg);

  /* get the register values. */
  int val = get_int_reg(svm, reg);

  if (getenv("DEBUG") != NULL)
    printf("[STDOUT] Register R%02d => %d [Hex:%04x]\n", reg, val, val);
  else printf("0x%04X -> %d", val, val);


  /* handle the next instruction */
  svm->ip += 1;
}

/**
* Convert the number values of a register to a string
*/
void op_int_tostring(svm_t *svm) {
  /* get the register number to convert */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  if (getenv("DEBUG") != NULL) printf("INT_TOSTRING (register %d)\n", reg);

  /* get the values of the register */
  int cur = get_int_reg(svm, reg);

  /* allocate a buffer. */
  svm->registers[reg].type = STRING;
  svm->registers[reg].value.string = malloc(10);

  /* store the string-value */
  memset(svm->registers[reg].value.string, '\0', 10);
  sprintf(svm->registers[reg].value.string, "%d", cur);

  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Generate a random number and store in the specified register.
*/
void op_int_random(svm_t *svm) {
  /* get the register to save the output to */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  if (getenv("DEBUG") != NULL) printf("INT_RANDOM (register %d)\n", reg);


  /**
  * If we already have a string in the register delete it.
  */
  if ((svm->registers[reg].type == STRING) && (svm->registers[reg].value.string)) {
    free(svm->registers[reg].value.string);
  }

  /* set the value. */
  svm->registers[reg].type = NUMBER;
  svm->registers[reg].value.number = rand() % 0xFFFF;

  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Store a string in a register.
*/
void op_string_store(svm_t *svm) {
  /* get the destination register */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  /* get the string to store */
  char *str = string_from_stack(svm);

  /**
  * If we already have a string in the register delete it.
  */
  if ((svm->registers[reg].type == STRING) && (svm->registers[reg].value.string)) {
    free(svm->registers[reg].value.string);
  }

  /**
  * Now store the new string.
  */
  svm->registers[reg].type = STRING;
  svm->registers[reg].value.string = str;

  if (getenv("DEBUG") != NULL)
    printf("STRING_STORE (register %d) = '%s'\n", reg, str);

  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Print the (string) values of a register.
*/
void op_string_print(svm_t *svm) {
  /* get the reg number to print */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  if (getenv("DEBUG") != NULL)
    printf("STRING_PRINT (register %d)\n", reg);

  /* get the values of the register */
  char *str = get_string_reg(svm, reg);

  /* print */
  if (getenv("DEBUG") != NULL) printf("[stdout] register R%02d => %s\n", reg, str);
  else printf("%s", str);

  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Concatenate two strings, and store the result.
*/
void op_string_concat(svm_t *svm) {
  /* get the destination register */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  /* get the source register */
  unsigned int src1 = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  /* get the source register */
  unsigned int src2 = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  if (getenv("DEBUG") != NULL)
    printf("STRING_CONCAT (register:%d = Register:%d + Register:%d)\n",
    reg, src1, src2);

  /*
  * Ensure both source registers have string values.
  */
  char *str1 = get_string_reg(svm, src1);
  char *str2 = get_string_reg(svm, src2);

  /**
  * Allocate RAM for two strings.
  */
  int len = strlen(str1) + strlen(str2) + 1;

  /**
  * Zero.
  */
  char *tmp = malloc(len);
  memset(tmp, '\0', len);

  /**
  * Assign.
  */
  sprintf(tmp, "%s%s", str1, str2);


  /* if the destination-register currently contains a string .. free it */
  if ((svm->registers[reg].type == STRING) && (svm->registers[reg].value.string))
    free(svm->registers[reg].value.string);

  svm->registers[reg].value.string = tmp;
  svm->registers[reg].type = STRING;

  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Invoke the C system() function against a string register.
*/
void op_string_system(svm_t *svm) {
  /* get the reg */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  if (getenv("DEBUG") != NULL)
    printf("STRING_SYSTEM (register %d)\n", reg);

  /* Get the value we're to execute */
  char *str = get_string_reg(svm, reg);

  if (getenv("FUZZ") != NULL) {
    printf("Fuzzing - skipping execution of: %s\n", str);
    return;
  }

  int result __attribute__((unused));
  result = system(str);

  /* handle the next instruction */
  svm->ip += 1;
}

/**
* Convert a string to an int.
*/
void op_string_toint(svm_t *svm) {
  /* get the destination register */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  if (getenv("DEBUG") != NULL)
    printf("STRING_TOINT (register:%d)\n", reg);

  /* get the string and convert to number */
  char *str = get_string_reg(svm, reg);
  int i = atoi(str);

  /* free the old version */
  free(svm->registers[reg].value.string);

  /* set the int. */
  svm->registers[reg].type = NUMBER;
  svm->registers[reg].value.number = i;

  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Unconditional jump
*/
void op_jump_to(svm_t *svm) {
  /**
  * Read the two bytes which will build up the destination
  */
  unsigned int off1 = next_byte(svm);
  unsigned int off2 = next_byte(svm);

  /**
  * Convert to the offset in our code-segment.
  */
  int offset = BYTES_TO_ADDR(off1, off2);

  if (getenv("DEBUG") != NULL)
    printf("JUMP_TO(Offset:%d [Hex:%04X]\n", offset, offset);

  svm->ip = offset;
}


/**
* Jump to the given address if the Z-flag is set.
*/
void op_jump_z(svm_t *svm) {
  /**
  * Read the two bytes which will build up the destination
  */
  unsigned int off1 = next_byte(svm);
  unsigned int off2 = next_byte(svm);

  /**
  * Convert to the offset in our code-segment.
  */
  int offset = BYTES_TO_ADDR(off1, off2);

  if (getenv("DEBUG") != NULL)
    printf("JUMP_Z(Offset:%d [Hex:%04X]\n", offset, offset);


  if (svm->flags.z) svm->ip = offset;
  else {
    /* handle the next instruction */
    svm->ip += 1;
  }

}


/**
* Jump to the given address if the Z flag is NOT set.
*/
void op_jump_nz(svm_t *svm) {
  /**
  * Read the two bytes which will build up the destination
  */
  unsigned int off1 = next_byte(svm);
  unsigned int off2 = next_byte(svm);

  /**
  * Convert to the offset in our code-segment.
  */
  int offset = BYTES_TO_ADDR(off1, off2);

  if (getenv("DEBUG") != NULL)
    printf("JUMP_NZ(Offset:%d [Hex:%04X]\n", offset, offset);

  if (!svm->flags.z) svm->ip = offset;
  else {
    /* handle the next instruction */
    svm->ip += 1;
  }
}


MATH_OPERATION(op_math_add, +)    // reg_result = reg1 + reg2 ;
MATH_OPERATION(op_math_and, &)   // reg_result = reg1 & reg2 ;
MATH_OPERATION(op_math_sub, -)   // reg_result = reg1 - reg2 ;
MATH_OPERATION(op_math_mul, *)   // reg_result = reg1 * reg2 ;
MATH_OPERATION(op_math_xor, ^)   // reg_result = reg1 ^ reg2 ;
MATH_OPERATION(op_math_rgt, >>)   // reg_result = reg1 >> reg2 ;
MATH_OPERATION(op_math_lft, <<)   // reg_result = reg1 << reg2 ;
MATH_OPERATION(op_math_or, |)    // reg_result = reg1 | reg2 ;
/**
* Increment the given (number) register.
*/
void op_math_inc(svm_t *svm) {
  /* get the register number to increment */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  if (getenv("DEBUG") != NULL)
    printf("INC_OP (register %d)\n", reg);

  /* get, incr, set */
  int cur = get_int_reg(svm, reg);
  cur += 1;
  svm->registers[reg].value.number = cur;

  if (svm->registers[reg].value.number == 0) svm->flags.z = 1;
  else svm->flags.z = 0;


  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Decrement the given (number) register.
*/
void op_math_dec(svm_t *svm) {
  /* get the register number to decrement */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  if (getenv("DEBUG") != NULL) printf("DEC_OP (register %d)\n", reg);

  /* get, decr, set */
  int cur = get_int_reg(svm, reg);
  cur -= 1;
  svm->registers[reg].value.number = cur;

  if (svm->registers[reg].value.number == 0) svm->flags.z = 1;
  else svm->flags.z = 0;


  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Compare two registers.  Set the Z-flag if equal.
*/
void op_cmp_reg(svm_t *svm) {
  /* get the source register */
  unsigned int reg1 = next_byte(svm);
  BOUNDS_TEST_REG(reg1);

  /* get the source register */
  unsigned int reg2 = next_byte(svm);
  BOUNDS_TEST_REG(reg2);

  if (getenv("DEBUG") != NULL)
      printf("CMP (register:%d vs Register:%d)\n", reg1, reg2);

  svm->flags.z = 0;

  if (svm->registers[reg1].type == svm->registers[reg2].type) {
    if (svm->registers[reg1].type == STRING) {
      if (strcmp(svm->registers[reg1].value.string,
          svm->registers[reg2].value.string) == 0)
          svm->flags.z = 1;
        } else {
          if (svm->registers[reg1].value.number ==
            svm->registers[reg2].value.number)
            svm->flags.z = 1;
          }
        }

        /* handle the next instruction */
        svm->ip += 1;
      }


/**
* Compare a register values with a constant number.
*/
void op_cmp_immediate(svm_t *svm) {
  /* get the source register */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  /* get the number to compare with */
  unsigned int val1 = next_byte(svm);
  unsigned int val2 = next_byte(svm);
  int val = BYTES_TO_ADDR(val1, val2);

  if (getenv("DEBUG") != NULL)
    printf("CMP_IMMEDIATE (register:%d vs %d [Hex:%04X])\n", reg, val, val);

  svm->flags.z = 0;

  int cur = (int) get_int_reg(svm, reg);

  if (cur == val)
    svm->flags.z = 1;

  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Compare a register values with the given string.
*/
void op_cmp_string(svm_t *svm) {
  /* get the source register */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  /* Now we get the string to compare against from the stack */
  char *str = string_from_stack(svm);

  /* get the string value from the register */
  char *cur = get_string_reg(svm, reg);

  if (getenv("DEBUG") != NULL)
    printf("Comparing register-%d ('%s') - with string '%s'\n", reg, cur, str);

  /* compare */
  if (strcmp(cur, str) == 0) svm->flags.z = 1;
  else svm->flags.z = 0;

  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Does the given register contain a string?  Set the Z-flag if so.
*/
void op_is_string(svm_t *svm) {
  /* get the register to test */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  if (getenv("DEBUG") != NULL) printf("is register %02X a string?\n", reg);

  if (svm->registers[reg].type == STRING) svm->flags.z = 1;
  else svm->flags.z = 0;

  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Does the given register contain an number?  Set the Z-flag if so.
*/
void op_is_number(svm_t *svm) {
  /* get the register to test */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  if (getenv("DEBUG") != NULL)
    printf("is register %02X an number?\n", reg);

  if (svm->registers[reg].type == NUMBER) svm->flags.z = 1;
  else svm->flags.z = 0;

  /* handle the next instruction */
  svm->ip += 1;
}

/**
* Read from a given address into the specified register.
*/
void op_peek(svm_t *svm) {
  /* get the destination register */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  /* get the address to read from the second register */
  unsigned int addr = next_byte(svm);
  BOUNDS_TEST_REG(addr);

  if (getenv("DEBUG") != NULL)
    printf("LOAD_FROM_RAM (register:%d will contain values of address %04X)\n",
    reg, addr);

  /* get the address from the register */
  int adr = get_int_reg(svm, addr);
  if (adr < 0 || adr > 0xffff)
    svm_panic(svm, "Reading from outside RAM");

  /* Read the value from RAM */
  int val = svm->code[adr];

  /* if the destination currently contains a string .. free it */
  if ((svm->registers[reg].type == STRING) && (svm->registers[reg].value.string))
    free(svm->registers[reg].value.string);

  svm->registers[reg].value.number = val;
  svm->registers[reg].type = NUMBER;

  /* handle the next instruction */
  svm->ip += 1;
}

/**
* Write a register-values to memory.
*/
void op_poke(svm_t *svm) {
  /* get the destination register */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  /* get the address to write to from the second register */
  unsigned int addr = next_byte(svm);
  BOUNDS_TEST_REG(addr);

  /* Get the value we're to store. */
  int val = get_int_reg(svm, reg);

  /* Get the address we're to store it in. */
  int adr = get_int_reg(svm, addr);


  if (getenv("DEBUG") != NULL)
    printf("STORE_IN_RAM(Address %04X set to %02X)\n", adr, val);

  if (adr < 0 || adr > 0xffff)
    svm_panic(svm, "Writing outside RAM");

  /* do the necessary */
  svm->code[adr] = val;

  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Copy a chunk of memory.
*/
void op_memcpy(svm_t *svm) {
  /* get the register number to store to */
  unsigned int dest_reg = next_byte(svm);
  BOUNDS_TEST_REG(dest_reg);

  /* get the register number to copy from */
  unsigned int src_reg = next_byte(svm);
  BOUNDS_TEST_REG(src_reg);

  /* get the register number with the size */
  unsigned int size_reg = next_byte(svm);
  BOUNDS_TEST_REG(size_reg);

  /**
  * Now handle the copy.
  */
  int src = get_int_reg(svm, src_reg);
  int dest = get_int_reg(svm, dest_reg);
  int size = get_int_reg(svm, size_reg);

  if ( ( src <0 ) || ( dest < 0 ) ) {
    svm_panic(svm, "cannot copy to/from negative addresses");
    return;
  }

  if (getenv("DEBUG") != NULL) {
    printf("Copying %4x bytes from %04x to %04X\n", size, src, dest);
  }

  /** Slow, but copes with nulls and allows debugging. */
  for (int i = 0; i < size; i++)
  {
    int sc = src + i;
    int dt = dest + i;

    /*
    * Handle wrap-around.
    *
    * So copying 0x00FF bytes from 0xFFFE will actually
    * wrap around to 0x00FE.
    */
    while( sc >= 0xFFFF )
    sc -= 0xFFFF;
    while( dt >= 0xFFFF )
    dt -= 0xFFFF;


    if (getenv("DEBUG") != NULL) {
      printf("\tCopying from: %04x Copying-to %04X\n", sc, dt);
    }

    svm->code[dt] = svm->code[sc];
  }

  /* handle the next instruction */
  svm->ip += 1;
}

/**
* Push the values of a given register onto the stack.
*/
void op_stack_push(svm_t *svm) {
  /* get the register to push */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  /* Get the value we're to store. */
  int val = get_int_reg(svm, reg);

  if (getenv("DEBUG") != NULL)
    printf("PUSH (register %d [=%04x])\n", reg, val);

  /* store it */
  svm->sp += 1;
  svm->stack[svm->sp] = val;

  /**
  * Ensure the stack hasn't overflown.
  */
  int sp_size = sizeof(svm->stack) / sizeof(svm->stack[0]);
  if (svm->sp >= sp_size)
    svm_panic(svm, "stack overflow - stack is full");


  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Pop the topmost entry from the stack into the given register.
*/
void op_stack_pop(svm_t *svm) {
  /* get the register to pop */
  unsigned int reg = next_byte(svm);
  BOUNDS_TEST_REG(reg);

  /* ensure we're not outside the stack. */
  if (svm->sp <= 0)
    svm_panic(svm, "stack overflow - stack is empty");

  /* Get the value from the stack. */
  int val = svm->stack[svm->sp];
  svm->sp -= 1;

  if (getenv("DEBUG") != NULL)
    printf("POP (register %d) => %04x\n", reg, val);


  /* if the register stores a string .. free it */
  if ((svm->registers[reg].type == STRING) && (svm->registers[reg].value.string))
    free(svm->registers[reg].value.string);

  svm->registers[reg].value.number = val;
  svm->registers[reg].type = NUMBER;


  /* handle the next instruction */
  svm->ip += 1;
}


/**
* Return from a call - by popping the return address from the stack
* and jumping to it.
*/
void op_stack_ret(svm_t *svm) {
  /* ensure we're not outside the stack. */
  if (svm->sp <= 0)
    svm_panic(svm, "stack overflow - stack is empty");

  /* Get the value from the stack. */
  int val = svm->stack[svm->sp];
  svm->sp -= 1;

  if (getenv("DEBUG") != NULL)
    printf("RET() => %04x\n", val);


  /* update our instruction pointer. */
  svm->ip = val;

}


/**
* Call a routine - push the return address onto the stack.
*/
void op_stack_call(svm_t *svm) {
  /**
  * Read the two bytes which will build up the destination
  */
  unsigned int off1 = next_byte(svm);
  unsigned int off2 = next_byte(svm);

  /**
  * Convert to the offset in our code-segment.
  */
  int offset = BYTES_TO_ADDR(off1, off2);


  int sp_size = sizeof(svm->stack) / sizeof(svm->stack[0]);
  if (svm->sp >= sp_size)
    svm_panic(svm, "stack overflow - stack is full!");

  /**
  * Now we've got to save the address past this instruction
  * on the stack so that the "ret(urn)" instruction will go
  * to the correct place.
  */
  svm->sp += 1;


  svm->stack[svm->sp] = svm->ip + 1;

  /**
  * Now we've saved the return-address we can update the IP
  */
  svm->ip = offset;

}

/**
** End implementation of virtual machine op_codes.
**
**/



/**
* Map the op_codes to the handlers.
*/
void op_code_init(svm_t * svm) {
  /**
  * Initialize the random seed for the rendom opcode (INT_RANDOM)
  */
  srand(time(NULL));

  /**
  * All instructions will default to unknown.
  */
  for (int i = 0; i <= 255; i++)
  svm->op_codes[i] = op_unknown;

  /* early op_codes */
  svm->op_codes[EXIT] = op_exit;
  svm->op_codes[INT_STORE] = op_int_store;
  svm->op_codes[INT_PRINT] = op_int_print;
  svm->op_codes[INT_TOSTRING] = op_int_tostring;
  svm->op_codes[INT_RANDOM] = op_int_random;

  /* jumps */
  svm->op_codes[JUMP_TO] = op_jump_to;
  svm->op_codes[JUMP_NZ] = op_jump_nz;
  svm->op_codes[JUMP_Z] = op_jump_z;

  /* math */
  svm->op_codes[MATH_ADD] = op_math_add;
  svm->op_codes[MATH_AND] = op_math_and;
  svm->op_codes[MATH_SUB] = op_math_sub;
  svm->op_codes[MATH_MUL] = op_math_mul;
  svm->op_codes[MATH_DIV] = op_math_div;
  svm->op_codes[MATH_XOR] = op_math_xor;
  svm->op_codes[MATH_OR]  = op_math_or;
  svm->op_codes[MATH_INC] = op_math_inc;
  svm->op_codes[MATH_DEC] = op_math_dec;

  /* strings */
  svm->op_codes[STRING_STORE] = op_string_store;
  svm->op_codes[STRING_PRINT] = op_string_print;
  svm->op_codes[STRING_CONCAT] = op_string_concat;
  svm->op_codes[STRING_SYSTEM] = op_string_system;
  svm->op_codes[STRING_TOINT] = op_string_toint;

  /* comparisons/tests */
  svm->op_codes[CMP_REG] = op_cmp_reg;
  svm->op_codes[CMP_IMMEDIATE] = op_cmp_immediate;
  svm->op_codes[CMP_STRING] = op_cmp_string;
  svm->op_codes[IS_STRING] = op_is_string;
  svm->op_codes[IS_NUMBER] = op_is_number;

  /* misc */
  svm->op_codes[NOP] = op_nop;
  svm->op_codes[STORE_REG] = op_reg_store;

  /* PEEK/POKE */
  svm->op_codes[PEEK] = op_peek;
  svm->op_codes[POKE] = op_poke;
  svm->op_codes[MEMCPY] = op_memcpy;

  /* stack */
  svm->op_codes[STACK_PUSH] = op_stack_push;
  svm->op_codes[STACK_POP] = op_stack_pop;
  svm->op_codes[STACK_RET] = op_stack_ret;
  svm->op_codes[STACK_CALL] = op_stack_call;
}
