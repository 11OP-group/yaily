#pragma once

#include "yaily/utils.h"

#define MAX_TOKEN_LINGTH 128

typedef enum {
  TOKEN_EOF = 0,       // Конец файла
  TOKEN_ERROR,         // Ошибка лексического анализа
                       //
  TOKEN_NEWLINE,       // \n
                       //
  TOKEN_IDENT_PASCAL,  // Point, Int, String
  TOKEN_IDENT_SNAKE,   // point, user_id, add
                       //
  TOKEN_ATOM,          // :atom, :ok, :error
                       //
  TOKEN_INTEGER,       // 42
  TOKEN_FLOAT,         // 0.5
  TOKEN_STRING,        // "Hello world"
                       //
  TOKEN_IF,            // if
  TOKEN_ELSE,          // else
  TOKEN_IMPORT,        // import
                       //
  TOKEN_LPAREN,        // (
  TOKEN_RPAREN,        // )
  TOKEN_LBRACE,        // {
  TOKEN_RBRACE,        // }
  TOKEN_LBRACKET,      // [
  TOKEN_RBRACKET,      // ]
                       //
  TOKEN_COMMA,         // ,
  TOKEN_DOT,           // .
  TOKEN_COLON,         // :
                       //
  TOKEN_EQUAL,         // =
  TOKEN_COLON_EQUAL,   // :=
                       //
  TOKEN_ARROW,         // ->
  TOKEN_FAT_ARROW,     // =>
                       //
  TOKEN_PLUS,          // +
  TOKEN_MINUS,         // -
  TOKEN_STAR,          // *
  TOKEN_SLASH,         // /
  TOKEN_PERCENT,       // %
                       //
  TOKEN_EQUAL_EQUAL,   // ==
  TOKEN_BANG_EQUAL,    // !=
  TOKEN_LESS,          // <
  TOKEN_GREATER,       // >
  TOKEN_LESS_EQUAL,    // <=
  TOKEN_GREATER_EQUAL, // >=
                       //
  TOKEN_PIPE,          // |
  TOKEN_PIPELINE,      // |>
                       //
  TOKEN_BANG,          // !
                       //
  TOKEN_HASH,          // #
  TOKEN_AT             // @
} TokenType;

// typedef struct {
//   const char *file_path;
//   int row;
//   int col;
// } Loc;

typedef struct {
  // const char *file_path;
  const char *text;
  TokenType type;
  int text_len;
  int row;
  int col;
} Token;

typedef struct {
  Token *data;
  int size;
  int capacity;
} VecToken;

typedef struct {
  const char *content;
  int content_len;
  int cursor; // abs pos
  int line;
  int bol; // begin of line
  // Lexer *l;
  // int col = l->cursor - l->bol;
} Lexer;

Lexer lexer_new(const char *content, int content_len);

Token lexer_next(Lexer *l);

/* maybe... void-generic DynArr func in utils.c... Need thinking...
void init_array(void *da, int initial_capacity);
void insert_array(void *da, void *el); // da->data[da->size] = *el;
void free_array(void *da);
*/
