//
// memory.c
//
// Copyright 2024 The Glim Authors and Contributors.
//
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "memory.h"
#include <stdlib.h>

void *memory_alloc(size_t size, Error *err)
{
  void *ptr = malloc(size);
  if (!ptr) error_set(err, "out of memory");
  return ptr;
}

void *memory_realloc(void *ptr, size_t size, Error *err)
{
  void *_ptr = realloc(ptr, size);
  if (!_ptr) error_set(err, "out of memory");
  return _ptr;
}

void memory_free(void *ptr)
{
  free(ptr);
}
