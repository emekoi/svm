/**
 * Copyright (c) 2017 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include "token.h"

typedef struct {
  const char *source;
  const char *token_start;
  const char *current;
  size_t line;
} plexer_t;

void lexer_init(const char *source);
ptoken_t lexer_get_token();

#endif
