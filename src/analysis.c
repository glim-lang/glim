//
// analysis.c
//
// Copyright 2024 The Glim Authors and Contributors.
//
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "analysis.h"
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

void analysis_init(Analysis *analysis, Error *err)
{
  slice_init(&analysis->messages, err);
}

void analysis_deinit(Analysis *analysis)
{
  slice_deinit(&analysis->messages);
}

void analysis_append(Analysis *analysis, Error *err, MessageKind kind, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  analysis_append_with_args(analysis, err, kind, fmt, args);
  va_end(args);
}

void analysis_append_with_args(Analysis *analysis, Error *err, MessageKind kind,
  const char *fmt, va_list args)
{
  char str[MESSAGE_MAX_LENGTH + 1];
  vsnprintf(str, MESSAGE_MAX_LENGTH, fmt, args);
  str[MESSAGE_MAX_LENGTH] = '\0';
  Message msg;
  msg.kind = kind;
  copy_cstring(msg.str, str, MESSAGE_MAX_LENGTH);
  slice_append(&analysis->messages, msg, err);
}

void analysis_print(Analysis *analysis)
{
  for (size_t i = 0; i < analysis->messages.len; ++i)
  {
    Message *msg = &slice_get(&analysis->messages, i);
    const char *kindName = message_kind_name(msg->kind);
    printf("%s: %s\n", kindName, msg->str);
  }
}
