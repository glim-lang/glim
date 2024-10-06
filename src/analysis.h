//
// analysis.h
//
// Copyright 2024 The Glim Authors and Contributors.
//
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef ANALYSIS_H
#define ANALYSIS_H

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
} Analysis;

void analysis_init(Analysis *analysis, Error *err);
void analysis_deinit(Analysis *analysis);
void analysis_append(Analysis *analysis, Error *err, MessageKind kind, const char *fmt, ...);
void analysis_append_with_args(Analysis *analysis, Error *err, MessageKind kind,
  const char *fmt, va_list args);
void analysis_print(Analysis *analysis);

#endif // ANALYSIS_H
