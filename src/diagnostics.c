//
// diagnostics.c
//
// Copyright 2024 The Glim Authors and Contributors.
//
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "diagnostics.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

static inline const char *message_kind_name(MessageKind kind);

static inline const char *message_kind_name(MessageKind kind)
{
  char *name = NULL;
  switch (kind)
  {
  case MESSAGE_KIND_NOTE:    name = "NOTE";    break;
  case MESSAGE_KIND_WARNING: name = "WARNING"; break;
  case MESSAGE_KIND_ERROR:   name = "ERROR";   break;
  }
  assert(name);
  return name;
}

void diagnostics_init(Diagnostics *diag, Error *err)
{
  slice_init(&diag->messages, err);
}

void diagnostics_deinit(Diagnostics *diag)
{
  slice_deinit(&diag->messages);
}

void diagnostics_append(Diagnostics *diag, Error *err, MessageKind kind, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  diagnostics_append_with_args(diag, err, kind, fmt, args);
  va_end(args);
}

void diagnostics_append_with_args(Diagnostics *diag, Error *err, MessageKind kind,
  const char *fmt, va_list args)
{
  char str[MESSAGE_MAX_LENGTH + 1];
  vsnprintf(str, MESSAGE_MAX_LENGTH, fmt, args);
  str[MESSAGE_MAX_LENGTH] = '\0';
  Message msg;
  msg.kind = kind;
  copy_cstring(msg.str, str, MESSAGE_MAX_LENGTH);
  slice_append(&diag->messages, msg, err);
}

void diagnostics_print(Diagnostics *diag)
{
  for (size_t i = 0; i < diag->messages.len; ++i)
  {
    Message *msg = &slice_get(&diag->messages, i);
    const char *kindName = message_kind_name(msg->kind);
    printf("%s: %s\n", kindName, msg->str);
  }
}
