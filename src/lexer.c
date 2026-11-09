#include "yaily/lexer.h"
#include "yaily/utils.h"
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
char peek(Lexer *l) { return l->content[l->cursor]; }

bool is_at_end(Lexer *l) { return l->content[l->cursor] == '\0'; }

bool match(Lexer *l, char expected) {
  if (is_at_end(l)) {
    return false;
  }

  if (l->content[l->cursor] != expected) {
    return false;
  }

  l->cursor++;

  return true;
}

void left_trim(Lexer *l) {
  for (;;) {
    char c = peek(l);
    switch (c) {
    case ' ':
    case '\t':
    case '\r':
      advance(l);
      break;
    default:
      return;
    }
  }
}

Token string(Lexer *l) {
  int len = 2;

  while (peek(l) != '"' && !is_at_end(l)) {
    len++;

    if (peek(l) == '\n') {
      l->line++;
      l->bol = l->cursor;
    }

    advance(l);
  }

  if (is_at_end(l)) {
    return token_error(l, "Unterminated string.");
  }

  // close quote
  advance(l);

  return token_new(l, TOKEN_STRING, len);
}

Token lexer_next(Lexer *l) {
  left_trim(l);

  if (is_at_end(l)) {
    return (Token){
        .text = "EOF",
        .type = TOKEN_EOF,
        .text_len = 3,
        .row = l->line,
        .col = l->cursor - l->bol,
    };
  }

  char c = advance(l);

  switch (c) {
  case '/':
    if (match(l, '/') && !is_at_end(l)) {
      while (peek(l) != '\n' && !is_at_end(l)) {
        advance(l);
      }
      return (Token){
          .text = "COMMENT",
          .type = TOKEN_SKIP,
          .text_len = 7,
          .row = l->line,
          .col = l->cursor - l->bol,
      };
    } else {
      return token_new(l, TOKEN_SLASH, 1);
    }
    break;

  case '\n':;
    Token t = {
        .text = "NEWLINE",
        .type = TOKEN_NEWLINE,
        .text_len = 7,
        .row = l->line,
        .col = l->cursor - l->bol,
    };
    l->line++;
    l->bol = l->cursor;
    return t;

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
    if (match(l, '=')) {
      return token_new(l, TOKEN_COLON_EQUAL, 2);
    }
    return token_new(l, TOKEN_COLON, 1);

  case '=':
    if (match(l, '=')) {
      return token_new(l, TOKEN_EQUAL_EQUAL, 2);
    }
    if (match(l, '>')) {
      return token_new(l, TOKEN_FAT_ARROW, 2);
    }
    return token_new(l, TOKEN_EQUAL, 1);

  case '+':
    return token_new(l, TOKEN_PLUS, 1);
  case '-':
    if (match(l, '>')) {
      return token_new(l, TOKEN_ARROW, 2);
    }
    return token_new(l, TOKEN_MINUS, 1);
  case '*':
    return token_new(l, TOKEN_STAR, 1);
  // case '/':
  //   return token_new(l, TOKEN_SLASH, 1);
  case '%':
    return token_new(l, TOKEN_PERCENT, 1);

  case '<':
    if (match(l, '=')) {
      return token_new(l, TOKEN_LESS_EQUAL, 2);
    }
    return token_new(l, TOKEN_LESS, 1);
  case '>':
    if (match(l, '=')) {
      return token_new(l, TOKEN_GREATER_EQUAL, 2);
    }
    return token_new(l, TOKEN_GREATER, 1);
  case '|':
    if (match(l, '>')) {
      return token_new(l, TOKEN_PIPELINE, 2);
    }
    return token_new(l, TOKEN_PIPE, 1);

  case '!':
    if (match(l, '=')) {
      return token_new(l, TOKEN_BANG_EQUAL, 2);
    }
    return token_new(l, TOKEN_BANG, 1);

  case '"':
    return string(l);

    // case ':':
    //   return atom();
    // case '\'':
    // case '#':
    //   return macro();
    // case '@':
    //   return deco();
  }

  // printf("\tNot impl: lexer_next\n");
  return token_error(l, "Unexpected character.");
}
