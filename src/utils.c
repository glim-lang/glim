//
// utils.c
//
// Copyright 2024 The Glim Authors and Contributors.
//
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "utils.h"
#include <string.h>

void copy_cstring(char *dest, const char *src, int n)
{
#ifdef _WIN32
  strncpy_s(dest, n, src, _TRUNCATE);
#else
  strncpy(dest, src, n);
#endif
}
