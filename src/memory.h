//
// memory.h
//
// Copyright 2024 The Glim Authors and Contributors.
//
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include "error.h"

void *memory_alloc(size_t size, Error *err);
void *memory_realloc(void *ptr, size_t size, Error *err);
void memory_free(void *ptr);

#endif // MEMORY_H
