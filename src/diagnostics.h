//
// diagnostics.h
//
// Copyright 2024 The Glim Authors and Contributors.
//
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include "slice.h"

#define MESSAGE_MAX_LENGTH (511)

typedef enum
{
  MESSAGE_KIND_NOTE,
  MESSAGE_KIND_WARNING,
  MESSAGE_KIND_ERROR
} MessageKind;

typedef struct
{
  MessageKind kind;
  char        str[MESSAGE_MAX_LENGTH + 1];
} Message;

typedef struct
{
  Slice(Message) messages;
} Diagnostics;

void diagnostics_init(Diagnostics *diag, Error *err);
void diagnostics_deinit(Diagnostics *diag);
void diagnostics_append(Diagnostics *diag, Error *err, MessageKind kind, const char *fmt, ...);
void diagnostics_append_with_args(Diagnostics *diag, Error *err, MessageKind kind,
  const char *fmt, va_list args);
void diagnostics_print(Diagnostics *diag);

#endif // DIAGNOSTICS_H
