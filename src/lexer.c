#include "yaily/lexer.h"
#include "yaily/utils.h"
#include <stdatomic.h>
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
      .col = l->cursor - l->bol - text_len,
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

bool is_at_end(Lexer *l) { return l->cursor >= l->content_len; }
bool is_digit(char c) { return c >= '0' && c <= '9'; }

bool is_pascal_case(char c) {
  return (c >= 'a' && c <= 'z') || //
         (c >= 'A' && c <= 'Z') || //
         (c >= '0' && c <= '9');
}

bool is_snake_case(char c) {
  return (c >= 'a' && c <= 'z') || //
         (c >= '0' && c <= '9') || //
         c == '_';                 //
}

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

char advance(Lexer *l) { return l->content[l->cursor++]; }

char peek(Lexer *l) {
  if (is_at_end(l)) {
    return '\0';
  }

  return l->content[l->cursor];
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

Token token_string(Lexer *l) {
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

Token token_float(Lexer *l, int len) {
  advance(l);
  len++;

  while (is_digit(peek(l))) {
    len++;
    advance(l);
  }

  return token_new(l, TOKEN_FLOAT, len);
}

Token token_number(Lexer *l) {
  int len = 1;

  while (is_digit(peek(l))) {
    len++;
    advance(l);
  }

  if (peek(l) == '.') {
    return token_float(l, len);
  }

  return token_new(l, TOKEN_INTEGER, len);
}

Token token_ident_pascal(Lexer *l) {
  int len = 1;

  while (is_pascal_case(peek(l))) {
    len++;
    advance(l);
  }

  return token_new(l, TOKEN_IDENT_PASCAL, len);
}

Token token_ident_snake(Lexer *l) {
  int len = 1;

  while (is_snake_case(peek(l))) {
    len++;
    advance(l);
  }

  return token_new(l, TOKEN_IDENT_SNAKE, len);
}

Token check_keyword(Token t) {
  if (cmp_string(t.text, t.text_len, "if", 2)) {
    t.type = TOKEN_IF;
  } else if (cmp_string(t.text, t.text_len, "else", 4)) {
    t.type = TOKEN_ELSE;
  } else if (cmp_string(t.text, t.text_len, "import", 6)) {
    t.type = TOKEN_IMPORT;
  }

  return t;
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

  if (is_digit(c)) {
    return token_number(l);
  }

  if (is_snake_case(c)) {
    return check_keyword(token_ident_snake(l));
  }

  if (is_pascal_case(c)) {
    return token_ident_pascal(l);
  }

  switch (c) {
  case '/':
    if (match(l, '/')) {
      int len = 2;
      while (peek(l) != '\n' && !is_at_end(l)) {
        advance(l);
        len++;
      }
      return (Token){
          .text = "COMMENT",
          .type = TOKEN_COMMENT,
          .text_len = 7,
          .row = l->line,
          .col = l->cursor - l->bol - len,
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
    return token_string(l);
  }

  return token_error(l, "Unexpected character.");
}
