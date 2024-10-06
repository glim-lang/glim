//
// compiler.h
//
// Copyright 2024 The Glim Authors and Contributors.
//
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef COMPILER_H
#define COMPILER_H

#include "diagnostics.h"

void compile(char *source, Error *err, Diagnostics *diag);

#endif // COMPILER_H
