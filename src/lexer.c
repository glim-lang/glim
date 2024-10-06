//
// lexer.c
//
// Copyright 2024 The Glim Authors and Contributors.
//
// This file is part of the Glim Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "lexer.h"
#include <ctype.h>
#include <string.h>

#define char_at(l, i) ((l)->curr[(i)])
#define current(l)    char_at(l, 0)

static inline void skip_space(Lexer *lex);
static inline void next_char(Lexer *lex);
static inline void next_chars(Lexer *lex, int length);
static inline bool match_char(Lexer *lex, char c, TokenKind kind);
static inline bool match_chars(Lexer *lex, const char *chars, TokenKind kind);
static inline bool match_keyword(Lexer *lex, const char *kw, TokenKind kind);
static inline bool match_number(Lexer *lex);
static inline bool match_string(Lexer *lex, Error *err);
static inline bool match_name(Lexer *lex);
static inline Token token(Lexer *lex, TokenKind kind, int length, char *chars);

static inline void skip_space(Lexer *lex)
{
  while (isspace(current(lex)))
    next_char(lex);
}

static inline void next_char(Lexer *lex)
{
  if (current(lex) == '\n')
  {
    ++lex->ln;
    lex->col = 1;
    ++lex->curr;
    return;
  }
  ++lex->col;
  ++lex->curr;
}

static inline void next_chars(Lexer *lex, int length)
{
  for (int i = 0; i < length; ++i)
    next_char(lex);
}

static inline bool match_char(Lexer *lex, char c, TokenKind kind)
{
  if (current(lex) != c)
    return false;
  lex->token = token(lex, kind, 1, lex->curr);
  next_char(lex);
  return true;
}

static inline bool match_chars(Lexer *lex, const char *chars, TokenKind kind)
{
  int length = (int) strlen(chars);
  if (memcmp(lex->curr, chars, length))
    return false;
  lex->token = token(lex, kind, length, lex->curr);
  next_chars(lex, length);
  return true;
}

static inline bool match_keyword(Lexer *lex, const char *kw, TokenKind kind)
{
  int length = (int) strlen(kw);
  if (strncmp(lex->curr, kw, length)
   || (isalnum(char_at(lex, length)))
   || (char_at(lex, length) == '_'))
    return false;
  lex->token = token(lex, kind, length, lex->curr);
  next_chars(lex, length);
  return true;
}

static inline bool match_number(Lexer *lex)
{
  int length = 0;
  if (char_at(lex, length) == '0')
    ++length;
  else
  {
    if (char_at(lex, length) < '1' || char_at(lex, length) > '9')
      return false;
    ++length;
    while (isdigit(char_at(lex, length)))
      ++length;
  }
  if (char_at(lex, length) == '.')
  {
    if (!isdigit(char_at(lex, length + 1)))
      goto end;
    length += 2;
    while (isdigit(char_at(lex, length)))
      ++length;
  }
  if (char_at(lex, length) == 'e' || char_at(lex, length) == 'E')
  {
    ++length;
    if (char_at(lex, length) == '+' || char_at(lex, length) == '-')
      ++length;
    if (!isdigit(char_at(lex, length)))
      return false;
    ++length;
    while (isdigit(char_at(lex, length)))
      ++length;
  }
  if (isalnum(char_at(lex, length)) || char_at(lex, length) == '_')
    return false;
end:
  lex->token = token(lex, TOKEN_KIND_NUMBER, length, lex->curr);
  next_chars(lex, length);
  return true;
}

static inline bool match_string(Lexer *lex, Error *err)
{
  if (current(lex) != '\"') return false;
  int n = 1;
  for (;;)
  {
    if (char_at(lex, n) == '\"')
    {
      ++n;
      break;
    }
    if (char_at(lex, n) == '\0')
    {
      error_set(err, "unterminated string [%d:%d]", lex->ln, lex->col);
      return false;
    }
    ++n;
  }
  lex->token = token(lex, TOKEN_KIND_STRING, n - 2, &lex->curr[1]);
  next_chars(lex, n);
  return true;
}

static inline bool match_name(Lexer *lex)
{
  if (current(lex) != '_' && !isalpha(current(lex)))
    return false;
  int length = 1;
  while (char_at(lex, length) == '_' || isalnum(char_at(lex, length)))
    ++length;
  lex->token = token(lex, TOKEN_KIND_NAME, length, lex->curr);
  next_chars(lex, length);
  return true;
}

static inline Token token(Lexer *lex, TokenKind kind, int length, char *chars)
{
  return (Token) {
    .kind = kind,
    .ln = lex->ln,
    .col = lex->col,
    .length = length,
    .chars = chars
  };
}

void lexer_init(Lexer *lex, char *source, Error *err)
{
  lex->source = source;
  lex->curr = source;
  lex->ln = 1;
  lex->col = 1;
  lex->err = err;
  lexer_next(lex);
}

void lexer_next(Lexer *lex)
{
  skip_space(lex);
  if (match_char(lex, 0, TOKEN_KIND_EOF)) return;
  if (match_char(lex, ',', TOKEN_KIND_COMMA)) return;
  if (match_char(lex, ':', TOKEN_KIND_COLON)) return;
  if (match_char(lex, ';', TOKEN_KIND_SEMICOLON)) return;
  if (match_char(lex, '?', TOKEN_KIND_QMARK)) return;
  if (match_char(lex, '(', TOKEN_KIND_LPAREN)) return;
  if (match_char(lex, ')', TOKEN_KIND_RPAREN)) return;
  if (match_char(lex, '[', TOKEN_KIND_LBRACKET)) return;
  if (match_char(lex, ']', TOKEN_KIND_RBRACKET)) return;
  if (match_chars(lex, "=>", TOKEN_KIND_EQGT)) return;
  if (match_chars(lex, "==", TOKEN_KIND_EQEQ)) return;
  if (match_char(lex, '=', TOKEN_KIND_EQ)) return;
  if (match_chars(lex, "|>", TOKEN_KIND_PIPEGT)) return;
  if (match_chars(lex, "||", TOKEN_KIND_PIPEPIPE)) return;
  if (match_chars(lex, "&&", TOKEN_KIND_AMPAMP)) return;
  if (match_chars(lex, "!=", TOKEN_KIND_BANGEQ)) return;
  if (match_char(lex, '!', TOKEN_KIND_BANG)) return;
  if (match_chars(lex, "<=", TOKEN_KIND_LTEQ)) return;
  if (match_char(lex, '<', TOKEN_KIND_LT)) return;
  if (match_chars(lex, ">=", TOKEN_KIND_GTEQ)) return;
  if (match_char(lex, '>', TOKEN_KIND_GT)) return;
  if (match_chars(lex, "++", TOKEN_KIND_PLUSPLUS)) return;
  if (match_char(lex, '+', TOKEN_KIND_PLUS)) return;
  if (match_char(lex, '-', TOKEN_KIND_MINUS)) return;
  if (match_char(lex, '*', TOKEN_KIND_STAR)) return;
  if (match_char(lex, '/', TOKEN_KIND_SLASH)) return;
  if (match_char(lex, '%', TOKEN_KIND_PERCENT)) return;
  if (match_number(lex)) return;
  if (match_string(lex, lex->err) || !lexer_ok(lex)) return;
  if (match_keyword(lex, "false", TOKEN_KIND_FALSE_KW)) return;
  if (match_keyword(lex, "let", TOKEN_KIND_LET_KW)) return;
  if (match_keyword(lex, "nil", TOKEN_KIND_NIL_KW)) return;
  if (match_keyword(lex, "true", TOKEN_KIND_TRUE_KW)) return;
  if (match_name(lex)) return;
  char c = current(lex);
  c = isprint(c) ? c : '?';
  const char *fmt = "unexpected character '%c' [%d:%d]";
  error_set(lex->err, fmt, c, lex->ln, lex->col);
}
