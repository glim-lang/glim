//
// error.h
//
// Copyright 2024 The Glim Authors and Contributors.
//
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef ERROR_H
#define ERROR_H

#include <stdarg.h>
#include <stdbool.h>

#define ERROR_MAX_LENGTH (511)

#define error_init(err) \
  do { \
    (err)->ok = true; \
  } while (0)

#define ok(err) ((err)->ok)

typedef struct
{
  bool ok;
  char str[ERROR_MAX_LENGTH + 1];
} Error;

void error_set(Error *err, const char *fmt, ...);
void error_set_with_args(Error *err, const char *fmt, va_list args);
void error_print(Error *err);

#endif // ERROR_H
