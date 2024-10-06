//
// slice.h
// 
// Copyright 2024 The Glim Authors and Contributors.
// 
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef SLICE_H
#define SLICE_H

#include "memory.h"

#define SLICE_MIN_CAPACITY 8

#define Slice(T) \
  struct { \
    size_t cap; \
    size_t len; \
    T      *slots; \
  }

#define slice_init(s, err) \
  do { \
    size_t cap = SLICE_MIN_CAPACITY; \
    size_t size = sizeof(*(s)->slots) * cap; \
    void *slots = memory_alloc(size, (err)); \
    if (!ok(err)) break; \
    (s)->cap = cap; \
    (s)->len = 0; \
    (s)->slots = slots; \
  } while (0)

#define slice_init_with_capacity(s, c, err) \
  do { \
    size_t _cap = SLICE_MIN_CAPACITY; \
    while (_cap < (c)) _cap <<= 1; \
    size_t size = sizeof(*(s)->slots) * _cap; \
    void *slots = memory_alloc(size, (err)); \
    if (!ok(err)) break; \
    (s)->cap = _cap; \
    (s)->len = 0; \
    (s)->slots = slots; \
  } while (0)

#define slice_deinit(s) \
  do { \
    memory_free((s)->slots); \
  } while (0)

#define slice_ensure_capacity(s, c, err) \
  do { \
    if ((c) <= (s)->cap) break; \
    size_t _cap = (s)->cap; \
    while (_cap < (c)) _cap <<= 1; \
    size_t size = sizeof(*(s)->slots) * _cap; \
    void *slots = memory_realloc((s)->slots, size, (err)); \
    if (!ok(err)) break; \
    (s)->cap = _cap; \
    (s)->slots = slots; \
  } while (0)

#define slice_is_empty(s) (!(s)->len)

#define slice_get(s, i) ((s)->slots[(i)])

#define slice_append(s, v, err) \
  do { \
    slice_ensure_capacity((s), (s)->len + 1, err); \
    if (!ok(err)) break; \
    (s)->slots[(s)->len] = (v); \
    ++(s)->len; \
  } while (0)

#define slice_set(s, i, v) \
  do { \
    (s)->slots[(i)] = (v); \
  } while (0)

#define slice_remove_at(s, i) \
  do { \
    --(s)->len; \
    for (size_t j = (i); j < (s)->len; ++j) \
      (s)->slots[j] = (s)->slots[j + 1]; \
  } while (0)

#define slice_clear(s) \
  do { \
    (s)->len = 0; \
  } while (0)

#endif // SLICE_H
