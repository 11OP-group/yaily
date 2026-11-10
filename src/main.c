#include "yaily/lexer.h"
#include "yaily/utils.h"
#include <stddef.h>
#include <stdio.h>

void interpret(char *code) {
  Lexer l = lexer_new(code, strlen(code));

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
    printf("Not impl: repl\n");
  }
}

char *read_file(char *path) {
  FILE *file = fopen(path, "rb");

  fseek(file, 0L, SEEK_END);
  size_t file_size = ftell(file);
  rewind(file);

  char *buffer = (char *)malloc(file_size + 1);
  size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
  buffer[bytes_read] = '\0';

  fclose(file);

  return buffer;
}

void run_file(char *path) {
  char *code = read_file(path);

  interpret(code);

  free(code);
  printf("Not implemented\nPath: %s", path);
}

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
