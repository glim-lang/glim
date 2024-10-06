//
// lexer.h
//
// Copyright 2024 The Glim Authors and Contributors.
//
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef LEXER_H
#define LEXER_H

#include "error.h"

#define lexer_ok(l) ok((l)->err)

typedef enum
{
  TOKEN_KIND_EOF,       TOKEN_KIND_COMMA,     TOKEN_KIND_COLON,     TOKEN_KIND_SEMICOLON,
  TOKEN_KIND_QMARK,     TOKEN_KIND_LPAREN,    TOKEN_KIND_RPAREN,    TOKEN_KIND_LBRACKET,
  TOKEN_KIND_RBRACKET,  TOKEN_KIND_EQGT,      TOKEN_KIND_EQEQ,      TOKEN_KIND_EQ,
  TOKEN_KIND_PIPEGT,    TOKEN_KIND_PIPEPIPE,  TOKEN_KIND_AMPAMP,    TOKEN_KIND_BANGEQ,
  TOKEN_KIND_BANG,      TOKEN_KIND_LTEQ,      TOKEN_KIND_LT,        TOKEN_KIND_GTEQ,
  TOKEN_KIND_GT,        TOKEN_KIND_PLUSPLUS,  TOKEN_KIND_PLUS,      TOKEN_KIND_MINUS,
  TOKEN_KIND_STAR,      TOKEN_KIND_SLASH,     TOKEN_KIND_PERCENT,   TOKEN_KIND_NUMBER,
  TOKEN_KIND_STRING,    TOKEN_KIND_FALSE_KW,  TOKEN_KIND_LET_KW,    TOKEN_KIND_NIL_KW,
  TOKEN_KIND_TRUE_KW,   TOKEN_KIND_NAME
} TokenKind;

typedef struct
{
  TokenKind kind;
  int       ln;
  int       col;
  int       length;
  char      *chars;
} Token;

typedef struct
{
  char  *source;
  char  *curr;
  int   ln;
  int   col;
  Error *err;
  Token token;
} Lexer;

void lexer_init(Lexer *lex, char *source, Error *err);
void lexer_next(Lexer *lex);

#endif // LEXER_H
