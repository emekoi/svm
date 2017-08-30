/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include "util.h"

int string_casencmp(const char *s1, const char *s2, size_t n) {
    while(n > 0 && ((unsigned char)*s1) == ((unsigned char)*s2)) {
      if(*s1 == '\0') return 0;
      s1++; s2++; n--;
    }

    if(n == 0) return 0;
    return ((unsigned char)*s1) - ((unsigned char)*s2);
}

char *dirname(char *str) {
  char *dir = calloc(strlen(str), sizeof(char));
  strcpy(dir, str);
  char *p = dir + strlen(dir);
  while (p != str) {
    if (*p == '/' || *p == '\\') {
      *p = '\0';
      break;
    }
    p--;
  }
  return dir;
}

char *basename(char *str) {
  char *p = str + strlen(str);
  char *file = "";
  while (p != str) {
    if (*p == '/' || *p == '\\') {
      UNUSED(*p++);
      file = p;
      break;
    }
    p--;
  }
  return file;
}

char *strip(char *s) {
  size_t size;
  char *end;
  size = strlen(s);
  if (!size)
    return s;
  end = s + size - 1;
  while (end >= s && isspace(*end))
    end--;
  *(end + 1) = '\0';
  while (*s && isspace(*s))
    s++;
  return s;
}

char *concat(char *str, ...) {
  va_list args;
  char *s;
  /* Get len */
  int len = strlen(str);
  va_start(args, str);
  while ((s = va_arg(args, char*))) {
    len += strlen(s);
  }
  va_end(args);
  /* Build string */
  char *res = malloc(len + 1);
  if (!res) return NULL;
  strcpy(res, str);
  va_start(args, str);
  while ((s = va_arg(args, char*))) strcat(res, s);
  va_end(args);
  return res;
}
