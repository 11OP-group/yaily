#include "yaily/lexer.h"
#include "yaily/utils.h"
#include <stdio.h>

void interpret(char *code) {
  Lexer l = lexer_new(code, 1024);

  int line = -1;
  for (;;) {
    Token token = lexer_next(&l);

    if (token.row != line) {
      printf("%4d ", token.row);
      line = token.row;
    } else {
      printf("   | ");
    }

    printf("%2d type: %2d '%.*s'\n", token.col, token.type, token.text_len,
           token.text);

    if (token.type == TOKEN_EOF) {
      break;
    }
  }

  printf("Not impl: repl\n");
  return;
}

void repl() {
  char line[1024];
  for (;;) {
    printf("> ");

    if (!fgets(line, sizeof(line), stdin)) {
      printf("\n");
      break;
    }

    interpret(line);
  }
}

void run_file(char *path) { printf("Not implemented\nPath: %s", path); }

int main(int argc, char *argv[]) {
  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    run_file(argv[1]);
  } else {
    fprintf(stderr, "Usage: yaily [path]\n");
  }

  return EXIT_SUCCESS;
}
