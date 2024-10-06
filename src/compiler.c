//
// compiler.c
//
// Copyright 2024 The Glim Authors and Contributors.
//
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "compiler.h"
#include "diagnostics.h"
#include "lexer.h"

#define current(p) ((p)->lex->token)

#define match(p, t) (current(p).kind == (t))

#define next(p) \
  do { \
    lexer_next((p)->lex); \
    if (!compiler_ok(p)) return; \
  } while (0)

#define consume(p, t) \
  do { \
    if (!match((p), (t))) { \
      unexpected_token_error(p); \
      return; \
    } \
    next(p); \
  } while (0)

#define compiler_ok(p) ok((p)->err)

typedef struct
{
  Lexer       *lex;
  Error       *err;
  Diagnostics *diag;
} Compiler;

static inline void compiler_init(Compiler *comp, Lexer *lex, Error *err, Diagnostics *diag);
static inline void unexpected_token_error(Compiler *comp);
static inline void compile_stmt(Compiler *comp);
static inline void compile_let_stmt(Compiler *comp);
static inline void compile_expr(Compiler *comp);
static inline void compile_ternary_expr(Compiler *comp);
static inline void compile_or_expr(Compiler *comp);
static inline void compile_and_expr(Compiler *comp);
static inline void compile_eq_expr(Compiler *comp);
static inline void compile_rel_expr(Compiler *comp);
static inline void compile_concat_expr(Compiler *comp);
static inline void compile_add_expr(Compiler *comp);
static inline void compile_mul_expr(Compiler *comp);
static inline void compile_unary_expr(Compiler *comp);
static inline void compile_subscr_expr(Compiler *comp);
static inline void compile_prim_expr(Compiler *comp);
static inline void compile_array_expr(Compiler *comp);
static inline void compile_name_expr(Compiler *comp);
static inline void compile_call(Compiler *comp);

static inline void compiler_init(Compiler *comp, Lexer *lex, Error *err, Diagnostics *diag)
{
  comp->lex = lex;
  comp->err = err;
  comp->diag = diag;
}

static inline void unexpected_token_error(Compiler *comp)
{
  Token *token = &comp->lex->token;
  if (token->kind == TOKEN_KIND_EOF)
  {
    error_set(comp->err, "unexpected end of file [%d:%d]", token->ln, token->col);
    return;
  }
  error_set(comp->err, "unexpected token '%.*s' [%d:%d]", token->length, token->chars,
    token->ln, token->col);
}

static inline void compile_stmt(Compiler *comp)
{
  if (match(comp, TOKEN_KIND_EOF))
    return;
  if (match(comp, TOKEN_KIND_LET_KW))
  {
    compile_let_stmt(comp);
    return;
  }
  compile_expr(comp);
  if (!compiler_ok(comp)) return;
  if (!match(comp, TOKEN_KIND_EOF))
    unexpected_token_error(comp);
}

static inline void compile_let_stmt(Compiler *comp)
{
  next(comp);
  if (!match(comp, TOKEN_KIND_NAME))
  {
    unexpected_token_error(comp);
    return;
  }
  Token name = current(comp);
  next(comp);
  (void) name;
  consume(comp, TOKEN_KIND_EQ);
  compile_expr(comp);
  if (!compiler_ok(comp)) return;
  consume(comp, TOKEN_KIND_SEMICOLON);
  compile_stmt(comp);
}

static inline void compile_expr(Compiler *comp)
{
  compile_ternary_expr(comp);
  if (!compiler_ok(comp)) return;
  while (match(comp, TOKEN_KIND_PIPEGT))
  {
    next(comp);
    compile_ternary_expr(comp);
    if (!compiler_ok(comp)) return;
  }
}

static inline void compile_ternary_expr(Compiler *comp)
{
  compile_or_expr(comp);
  if (!compiler_ok(comp)) return;
  if (!match(comp, TOKEN_KIND_QMARK))
    return;
  next(comp);
  compile_expr(comp);
  if (!compiler_ok(comp)) return;
  consume(comp, TOKEN_KIND_COLON);
  compile_expr(comp);
}

static inline void compile_or_expr(Compiler *comp)
{
  compile_and_expr(comp);
  if (!compiler_ok(comp)) return;
  while (match(comp, TOKEN_KIND_PIPEPIPE))
  {
    next(comp);
    compile_and_expr(comp);
    if (!compiler_ok(comp)) return;
  }
}

static inline void compile_and_expr(Compiler *comp)
{
  compile_eq_expr(comp);
  if (!compiler_ok(comp)) return;
  while (match(comp, TOKEN_KIND_AMPAMP))
  {
    next(comp);
    compile_eq_expr(comp);
    if (!compiler_ok(comp)) return;
  }
}

static inline void compile_eq_expr(Compiler *comp)
{
  compile_rel_expr(comp);
  if (!compiler_ok(comp)) return;
  for (;;)
  {
    if (match(comp, TOKEN_KIND_EQEQ))
    {
      next(comp);
      compile_rel_expr(comp);
      if (!compiler_ok(comp)) return;
      continue;
    }
    if (match(comp, TOKEN_KIND_BANGEQ))
    {
      next(comp);
      compile_rel_expr(comp);
      if (!compiler_ok(comp)) return;
      continue;
    }
    break;
  }
}

static inline void compile_rel_expr(Compiler *comp)
{
  compile_concat_expr(comp);
  if (!compiler_ok(comp)) return;
  for (;;)
  {
    if (match(comp, TOKEN_KIND_LT))
    {
      next(comp);
      compile_concat_expr(comp);
      if (!compiler_ok(comp)) return;
      continue;
    }
    if (match(comp, TOKEN_KIND_LTEQ))
    {
      next(comp);
      compile_concat_expr(comp);
      if (!compiler_ok(comp)) return;
      continue;
    }
    if (match(comp, TOKEN_KIND_GT))
    {
      next(comp);
      compile_concat_expr(comp);
      if (!compiler_ok(comp)) return;
      continue;
    }
    if (match(comp, TOKEN_KIND_GTEQ))
    {
      next(comp);
      compile_concat_expr(comp);
      if (!compiler_ok(comp)) return;
      continue;
    }
    break;
  }
}

static inline void compile_concat_expr(Compiler *comp)
{
  compile_add_expr(comp);
  if (!compiler_ok(comp)) return;
  while (match(comp, TOKEN_KIND_PLUSPLUS))
  {
    next(comp);
    compile_add_expr(comp);
    if (!compiler_ok(comp)) return;
  }
}

static inline void compile_add_expr(Compiler *comp)
{
  compile_mul_expr(comp);
  if (!compiler_ok(comp)) return;
  for (;;)
  {
    if (match(comp, TOKEN_KIND_PLUS))
    {
      next(comp);
      compile_mul_expr(comp);
      if (!compiler_ok(comp)) return;
      continue;
    }
    if (match(comp, TOKEN_KIND_MINUS))
    {
      next(comp);
      compile_mul_expr(comp);
      if (!compiler_ok(comp)) return;
      continue;
    }
    break;
  }
}

static inline void compile_mul_expr(Compiler *comp)
{
  compile_unary_expr(comp);
  if (!compiler_ok(comp)) return;
  for (;;)
  {
    if (match(comp, TOKEN_KIND_STAR))
    {
      next(comp);
      compile_unary_expr(comp);
      if (!compiler_ok(comp)) return;
      continue;
    }
    if (match(comp, TOKEN_KIND_SLASH))
    {
      next(comp);
      compile_unary_expr(comp);
      if (!compiler_ok(comp)) return;
      continue;
    }
    if (match(comp, TOKEN_KIND_PERCENT))
    {
      next(comp);
      compile_unary_expr(comp);
      if (!compiler_ok(comp)) return;
      continue;
    }
    break;
  }
}

static inline void compile_unary_expr(Compiler *comp)
{
  if (match(comp, TOKEN_KIND_BANG))
  {
    next(comp);
    compile_unary_expr(comp);
    return;
  }
  if (match(comp, TOKEN_KIND_MINUS))
  {
    next(comp);
    compile_unary_expr(comp);
    return;
  }
  compile_subscr_expr(comp);
}

static inline void compile_subscr_expr(Compiler *comp)
{
  compile_prim_expr(comp);
  if (!compiler_ok(comp)) return;
  for (;;)
  {
    if (match(comp, TOKEN_KIND_LBRACKET))
    {
      next(comp);
      compile_expr(comp);
      if (!compiler_ok(comp)) return;
      consume(comp, TOKEN_KIND_RBRACKET);
      continue;
    }
    if (match(comp, TOKEN_KIND_LPAREN))
    {
      compile_call(comp);
      if (!compiler_ok(comp)) return;
      continue;
    }
    break;
  }
}

static inline void compile_prim_expr(Compiler *comp)
{
  if (match(comp, TOKEN_KIND_NIL_KW))
  {
    next(comp);
    return;
  }
  if (match(comp, TOKEN_KIND_FALSE_KW))
  {
    next(comp);
    return;
  }
  if (match(comp, TOKEN_KIND_TRUE_KW))
  {
    next(comp);
    return;
  }
  if (match(comp, TOKEN_KIND_NUMBER))
  {
    Token num = current(comp);
    next(comp);
    (void) num;
    return;
  }
  if (match(comp, TOKEN_KIND_STRING))
  {
    Token str = current(comp);
    next(comp);
    (void) str;
    return;
  }
  if (match(comp, TOKEN_KIND_LBRACKET))
  {
    compile_array_expr(comp);
    return;
  }
  if (match(comp, TOKEN_KIND_NAME))
  {
    compile_name_expr(comp);
    return;
  }
  if (match(comp, TOKEN_KIND_LPAREN))
  {
    next(comp);
    compile_expr(comp);
    if (!compiler_ok(comp)) return;
    consume(comp, TOKEN_KIND_RPAREN);
    return;
  }
  unexpected_token_error(comp);
}

static inline void compile_array_expr(Compiler *comp)
{
  next(comp);
  if (match(comp, TOKEN_KIND_RBRACKET))
  {
    next(comp);
    return;
  }
  compile_expr(comp);
  if (!compiler_ok(comp)) return;
  while (match(comp, TOKEN_KIND_COMMA))
  {
    next(comp);
    compile_expr(comp);
    if (!compiler_ok(comp)) return;
  }
  consume(comp, TOKEN_KIND_RBRACKET);
}

static inline void compile_name_expr(Compiler *comp)
{
  Token name = current(comp);
  next(comp);
  (void) name;
  if (match(comp, TOKEN_KIND_EQGT))
  {
    next(comp);
    compile_expr(comp);
    return;
  }
  if (match(comp, TOKEN_KIND_COMMA))
  {
    next(comp);
    if (!match(comp, TOKEN_KIND_NAME))
    {
      unexpected_token_error(comp);
      return;
    }
    name = current(comp);
    next(comp);
    (void) name;
    while (match(comp, TOKEN_KIND_COMMA))
    {
      next(comp);
      if (!match(comp, TOKEN_KIND_NAME))
      {
        unexpected_token_error(comp);
        return;
      }
      name = current(comp);
      next(comp);
      (void) name;
    }
    consume(comp, TOKEN_KIND_EQGT);
    compile_expr(comp);
    return;
  }
}

static inline void compile_call(Compiler *comp)
{
  next(comp);
  if (match(comp, TOKEN_KIND_RPAREN))
  {
    next(comp);
    return;
  }
  compile_expr(comp);
  if (!compiler_ok(comp)) return;
  while (match(comp, TOKEN_KIND_COMMA))
  {
    next(comp);
    compile_expr(comp);
    if (!compiler_ok(comp)) return;
  }
  consume(comp, TOKEN_KIND_RPAREN);
}

void compile(char *source, Error *err, Diagnostics *diag)
{
  Lexer lex;
  lexer_init(&lex, source, err);
  if (!ok(err)) return;
  Compiler comp;
  compiler_init(&comp, &lex, err, diag);
  compile_stmt(&comp);
  if (!ok(err)) return;
  diagnostics_append(comp.diag, comp.err, MESSAGE_KIND_NOTE, "syntax is ok");
}
