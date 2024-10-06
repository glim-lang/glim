//
// error.c
//
// Copyright 2024 The Glim Authors and Contributors.
//
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "error.h"
#include <stdio.h>

void error_set(Error *err, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  error_set_with_args(err, fmt, args);
  va_end(args);
}

void error_set_with_args(Error *err, const char *fmt, va_list args)
{
  err->ok = false;
  vsnprintf(err->str, ERROR_MAX_LENGTH, fmt, args);
  err->str[ERROR_MAX_LENGTH] = '\0';
}

void error_print(Error *err)
{
  if (ok(err)) return;
  fprintf(stderr, "ERROR: %s\n", err->str);
}
