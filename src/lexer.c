#include "yaily/lexer.h"
#include "yaily/utils.h"
#include <ctype.h>
#include <string.h>

Lexer lexer_new(const char *content, int content_len) {
  Lexer l = {
      .content = content,
      .content_len = content_len,
      .line = 1,
      .cursor = 0,
      .bol = 0,
  };

  return l;
}

Token token_new(Lexer *l, TokenType type, int text_len) {
  return (Token){
      .text = &l->content[l->cursor - text_len],
      .type = type,
      .text_len = text_len,
      .row = l->line,
      .col = l->cursor - l->bol,
  };
}

Token token_error(Lexer *l, const char *message) {
  return (Token){
      .text = message,
      .type = TOKEN_ERROR,
      .text_len = (int)strlen(message),
      .row = l->line,
      .col = l->cursor - l->bol,
  };
}

char advance(Lexer *l) { return l->content[l->cursor++]; }

int is_at_end(Lexer *l) { return l->content[l->cursor] == '\0'; }

Token lexer_next(Lexer *l) {
  if (is_at_end(l)) {
    return token_new(l, TOKEN_EOF, 1);
  }

  char c = advance(l);

  switch (c) {
  case '(':
    return token_new(l, TOKEN_LPAREN, 1);
  case ')':
    return token_new(l, TOKEN_RPAREN, 1);
  case '{':
    return token_new(l, TOKEN_LBRACE, 1);
  case '}':
    return token_new(l, TOKEN_RBRACE, 1);
  case '[':
    return token_new(l, TOKEN_LBRACKET, 1);
  case ']':
    return token_new(l, TOKEN_RBRACKET, 1);

  case ',':
    return token_new(l, TOKEN_COMMA, 1);
  case '.':
    return token_new(l, TOKEN_DOT, 1);
  case ':':
    return token_new(l, TOKEN_COLON, 1);

  case '=':
    return token_new(l, TOKEN_EQUAL, 1);

  case '+':
    return token_new(l, TOKEN_PLUS, 1);
  case '-':
    return token_new(l, TOKEN_MINUS, 1);
  case '*':
    return token_new(l, TOKEN_STAR, 1);
  case '/':
    return token_new(l, TOKEN_SLASH, 1);
  case '%':
    return token_new(l, TOKEN_PERCENT, 1);

  case '<':
    return token_new(l, TOKEN_LESS, 1);
  case '>':
    return token_new(l, TOKEN_GREATER, 1);

  case '|':
    return token_new(l, TOKEN_PIPE, 1);

  case '!':
    return token_new(l, TOKEN_BANG, 1);
  }

  // printf("\tNot impl: lexer_next\n");
  return token_error(l, "Unexpected character.");
}
