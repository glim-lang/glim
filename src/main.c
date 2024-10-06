//
// main.c
// 
// Copyright 2024 The Glim Authors and Contributors.
// 
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include <stdlib.h>
#include "parser.h"

int main(void)
{
  char *source = "let fib = n => n <= 1 ? n : fib(n - 1) + fib(n - 2);\n"
                 "fib(10)";
  Error err;
  error_init(&err);
  Analysis analysis;
  analysis_init(&analysis, &err);
  if (!ok(&err)) goto error;
  parse(source, &err, &analysis);
  if (!ok(&err)) goto error;
  analysis_print(&analysis);
  return EXIT_SUCCESS;
error:
  error_print(&err);
  return EXIT_FAILURE;
}
