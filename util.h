/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#define color_red "\x1b[31m"
#define color_reset "\x1b[0m"

#define ASSERT(cond, ...) do { \
  if (!(cond)) { \
    fprintf(stderr, "%s[ASSERTION FAILED]:%s%s %s:%d: %s(): ", color_red, color_reset, #cond, __FILE__, __LINE__, __func__); \
  fprintf(stderr, __VA_ARGS__); \
  fprintf(stderr, "\n"); \
  exit(EXIT_FAILURE); \
  } \
} while(0);

#define ERROR(...) do { \
  fprintf(stderr, "%s[ERROR]:%s %s:%d %s(): ", color_red, color_reset, __FILE__, __LINE__, __func__); \
  fprintf(stderr, __VA_ARGS__); \
  fprintf(stderr, "\n"); \
  exit(EXIT_FAILURE); \
} while(0);

#define UNUSED(x)       ((void) (x))
#define MIN(a, b)       ((b) < (a) ? (b) : (a))
#define MAX(a, b)       ((b) > (a) ? (b) : (a))
#define CLAMP(x, a, b)  (MAX(a, MIN(x, b)))
#define LERP(a, b, p)   ((a) + ((b) - (a)) * (p))

#define tostring(e) #e

int string_casencmp(const char *s1, const char *s2, size_t n);

char *dirname(char *str);
char *basename(char *str);
char *strip(char *s);
char *concat(char *str, ...);

#endif
