//
// parser.c
//
// Copyright 2024 The Glim Authors and Contributors.
//
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "parser.h"
#include "analysis.h"
#include "lexer.h"

#define current(p) ((p)->lex->token)

#define match(p, t) (current(p).kind == (t))

#define next(p) \
  do { \
    lexer_next((p)->lex); \
    if (!parser_ok(p)) return; \
  } while (0)

#define consume(p, t) \
  do { \
    if (!match((p), (t))) { \
      unexpected_token_error(p); \
      return; \
    } \
    next(p); \
  } while (0)

#define parser_ok(p) ok((p)->err)

typedef struct
{
  Lexer    *lex;
  Error    *err;
  Analysis *analysis;
} Parser;

static inline void parser_init(Parser *parser, Lexer *lex, Error *err, Analysis *analysis);
static inline void unexpected_token_error(Parser *parser);
static inline void parse_stmt(Parser *parser);
static inline void parse_let_stmt(Parser *parser);
static inline void parse_expr(Parser *parser);
static inline void parse_ternary_expr(Parser *parser);
static inline void parse_or_expr(Parser *parser);
static inline void parse_and_expr(Parser *parser);
static inline void parse_eq_expr(Parser *parser);
static inline void parse_rel_expr(Parser *parser);
static inline void parse_concat_expr(Parser *parser);
static inline void parse_add_expr(Parser *parser);
static inline void parse_mul_expr(Parser *parser);
static inline void parse_unary_expr(Parser *parser);
static inline void parse_subscr_expr(Parser *parser);
static inline void parse_prim_expr(Parser *parser);
static inline void parse_array_expr(Parser *parser);
static inline void parse_name_expr(Parser *parser);
static inline void parse_call(Parser *parser);

static inline void parser_init(Parser *parser, Lexer *lex, Error *err, Analysis *analysis)
{
  parser->lex = lex;
  parser->err = err;
  parser->analysis = analysis;
}

static inline void unexpected_token_error(Parser *parser)
{
  Token *token = &parser->lex->token;
  if (token->kind == TOKEN_KIND_EOF)
  {
    error_set(parser->err, "unexpected end of file [%d:%d]", token->ln, token->col);
    return;
  }
  error_set(parser->err, "unexpected token '%.*s' [%d:%d]", token->length, token->chars,
    token->ln, token->col);
}

static inline void parse_stmt(Parser *parser)
{
  if (match(parser, TOKEN_KIND_EOF))
    return;
  if (match(parser, TOKEN_KIND_LET_KW))
  {
    parse_let_stmt(parser);
    return;
  }
  parse_expr(parser);
  if (!parser_ok(parser)) return;
  if (!match(parser, TOKEN_KIND_EOF))
    unexpected_token_error(parser);
}

static inline void parse_let_stmt(Parser *parser)
{
  next(parser);
  if (!match(parser, TOKEN_KIND_NAME))
  {
    unexpected_token_error(parser);
    return;
  }
  Token name = current(parser);
  next(parser);
  (void) name;
  consume(parser, TOKEN_KIND_EQ);
  parse_expr(parser);
  if (!parser_ok(parser)) return;
  consume(parser, TOKEN_KIND_SEMICOLON);
  parse_stmt(parser);
}

static inline void parse_expr(Parser *parser)
{
  parse_ternary_expr(parser);
  if (!parser_ok(parser)) return;
  while (match(parser, TOKEN_KIND_PIPEGT))
  {
    next(parser);
    parse_ternary_expr(parser);
    if (!parser_ok(parser)) return;
  }
}

static inline void parse_ternary_expr(Parser *parser)
{
  parse_or_expr(parser);
  if (!parser_ok(parser)) return;
  if (!match(parser, TOKEN_KIND_QMARK))
    return;
  next(parser);
  parse_expr(parser);
  if (!parser_ok(parser)) return;
  consume(parser, TOKEN_KIND_COLON);
  parse_expr(parser);
}

static inline void parse_or_expr(Parser *parser)
{
  parse_and_expr(parser);
  if (!parser_ok(parser)) return;
  while (match(parser, TOKEN_KIND_PIPEPIPE))
  {
    next(parser);
    parse_and_expr(parser);
    if (!parser_ok(parser)) return;
  }
}

static inline void parse_and_expr(Parser *parser)
{
  parse_eq_expr(parser);
  if (!parser_ok(parser)) return;
  while (match(parser, TOKEN_KIND_AMPAMP))
  {
    next(parser);
    parse_eq_expr(parser);
    if (!parser_ok(parser)) return;
  }
}

static inline void parse_eq_expr(Parser *parser)
{
  parse_rel_expr(parser);
  if (!parser_ok(parser)) return;
  for (;;)
  {
    if (match(parser, TOKEN_KIND_EQEQ))
    {
      next(parser);
      parse_rel_expr(parser);
      if (!parser_ok(parser)) return;
      continue;
    }
    if (match(parser, TOKEN_KIND_BANGEQ))
    {
      next(parser);
      parse_rel_expr(parser);
      if (!parser_ok(parser)) return;
      continue;
    }
    break;
  }
}

static inline void parse_rel_expr(Parser *parser)
{
  parse_concat_expr(parser);
  if (!parser_ok(parser)) return;
  for (;;)
  {
    if (match(parser, TOKEN_KIND_LT))
    {
      next(parser);
      parse_concat_expr(parser);
      if (!parser_ok(parser)) return;
      continue;
    }
    if (match(parser, TOKEN_KIND_LTEQ))
    {
      next(parser);
      parse_concat_expr(parser);
      if (!parser_ok(parser)) return;
      continue;
    }
    if (match(parser, TOKEN_KIND_GT))
    {
      next(parser);
      parse_concat_expr(parser);
      if (!parser_ok(parser)) return;
      continue;
    }
    if (match(parser, TOKEN_KIND_GTEQ))
    {
      next(parser);
      parse_concat_expr(parser);
      if (!parser_ok(parser)) return;
      continue;
    }
    break;
  }
}

static inline void parse_concat_expr(Parser *parser)
{
  parse_add_expr(parser);
  if (!parser_ok(parser)) return;
  while (match(parser, TOKEN_KIND_PLUSPLUS))
  {
    next(parser);
    parse_add_expr(parser);
    if (!parser_ok(parser)) return;
  }
}

static inline void parse_add_expr(Parser *parser)
{
  parse_mul_expr(parser);
  if (!parser_ok(parser)) return;
  for (;;)
  {
    if (match(parser, TOKEN_KIND_PLUS))
    {
      next(parser);
      parse_mul_expr(parser);
      if (!parser_ok(parser)) return;
      continue;
    }
    if (match(parser, TOKEN_KIND_MINUS))
    {
      next(parser);
      parse_mul_expr(parser);
      if (!parser_ok(parser)) return;
      continue;
    }
    break;
  }
}

static inline void parse_mul_expr(Parser *parser)
{
  parse_unary_expr(parser);
  if (!parser_ok(parser)) return;
  for (;;)
  {
    if (match(parser, TOKEN_KIND_STAR))
    {
      next(parser);
      parse_unary_expr(parser);
      if (!parser_ok(parser)) return;
      continue;
    }
    if (match(parser, TOKEN_KIND_SLASH))
    {
      next(parser);
      parse_unary_expr(parser);
      if (!parser_ok(parser)) return;
      continue;
    }
    if (match(parser, TOKEN_KIND_PERCENT))
    {
      next(parser);
      parse_unary_expr(parser);
      if (!parser_ok(parser)) return;
      continue;
    }
    break;
  }
}

static inline void parse_unary_expr(Parser *parser)
{
  if (match(parser, TOKEN_KIND_BANG))
  {
    next(parser);
    parse_unary_expr(parser);
    return;
  }
  if (match(parser, TOKEN_KIND_MINUS))
  {
    next(parser);
    parse_unary_expr(parser);
    return;
  }
  parse_subscr_expr(parser);
}

static inline void parse_subscr_expr(Parser *parser)
{
  parse_prim_expr(parser);
  if (!parser_ok(parser)) return;
  for (;;)
  {
    if (match(parser, TOKEN_KIND_LBRACKET))
    {
      next(parser);
      parse_expr(parser);
      if (!parser_ok(parser)) return;
      consume(parser, TOKEN_KIND_RBRACKET);
      continue;
    }
    if (match(parser, TOKEN_KIND_LPAREN))
    {
      parse_call(parser);
      if (!parser_ok(parser)) return;
      continue;
    }
    break;
  }
}

static inline void parse_prim_expr(Parser *parser)
{
  if (match(parser, TOKEN_KIND_NIL_KW))
  {
    next(parser);
    return;
  }
  if (match(parser, TOKEN_KIND_FALSE_KW))
  {
    next(parser);
    return;
  }
  if (match(parser, TOKEN_KIND_TRUE_KW))
  {
    next(parser);
    return;
  }
  if (match(parser, TOKEN_KIND_NUMBER))
  {
    Token num = current(parser);
    next(parser);
    (void) num;
    return;
  }
  if (match(parser, TOKEN_KIND_STRING))
  {
    Token str = current(parser);
    next(parser);
    (void) str;
    return;
  }
  if (match(parser, TOKEN_KIND_LBRACKET))
  {
    parse_array_expr(parser);
    return;
  }
  if (match(parser, TOKEN_KIND_NAME))
  {
    parse_name_expr(parser);
    return;
  }
  if (match(parser, TOKEN_KIND_LPAREN))
  {
    next(parser);
    parse_expr(parser);
    if (!parser_ok(parser)) return;
    consume(parser, TOKEN_KIND_RPAREN);
    return;
  }
  unexpected_token_error(parser);
}

static inline void parse_array_expr(Parser *parser)
{
  next(parser);
  if (match(parser, TOKEN_KIND_RBRACKET))
  {
    next(parser);
    return;
  }
  parse_expr(parser);
  if (!parser_ok(parser)) return;
  while (match(parser, TOKEN_KIND_COMMA))
  {
    next(parser);
    parse_expr(parser);
    if (!parser_ok(parser)) return;
  }
  consume(parser, TOKEN_KIND_RBRACKET);
}

static inline void parse_name_expr(Parser *parser)
{
  Token name = current(parser);
  next(parser);
  (void) name;
  if (match(parser, TOKEN_KIND_EQGT))
  {
    next(parser);
    parse_expr(parser);
    return;
  }
  if (match(parser, TOKEN_KIND_COMMA))
  {
    next(parser);
    if (!match(parser, TOKEN_KIND_NAME))
    {
      unexpected_token_error(parser);
      return;
    }
    name = current(parser);
    next(parser);
    (void) name;
    while (match(parser, TOKEN_KIND_COMMA))
    {
      next(parser);
      if (!match(parser, TOKEN_KIND_NAME))
      {
        unexpected_token_error(parser);
        return;
      }
      name = current(parser);
      next(parser);
      (void) name;
    }
    consume(parser, TOKEN_KIND_EQGT);
    parse_expr(parser);
    return;
  }
}

static inline void parse_call(Parser *parser)
{
  next(parser);
  if (match(parser, TOKEN_KIND_RPAREN))
  {
    next(parser);
    return;
  }
  parse_expr(parser);
  if (!parser_ok(parser)) return;
  while (match(parser, TOKEN_KIND_COMMA))
  {
    next(parser);
    parse_expr(parser);
    if (!parser_ok(parser)) return;
  }
  consume(parser, TOKEN_KIND_RPAREN);
}

void parse(char *source, Error *err, Analysis *analysis)
{
  Lexer lex;
  lexer_init(&lex, source, err);
  if (!ok(err)) return;
  Parser parser;
  parser_init(&parser, &lex, err, analysis);
  parse_stmt(&parser);
  if (!ok(err)) return;
  analysis_append(parser.analysis, parser.err, MESSAGE_KIND_NOTE, "syntax is ok");
}
