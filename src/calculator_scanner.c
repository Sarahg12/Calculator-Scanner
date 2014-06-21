#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "token.h"
#include "file_reader.h"

static int scan_token(char **buf, Token *token) {
  char *starting_buf = *buf;
  static int in_multiline_comment = false;
  char c = read_character(buf);

  if (in_multiline_comment) {
    if (c == '*') {
      if (read_character(buf) == '/') {
        in_multiline_comment = false;
        token_init(token, COMMENT, "/* ... */", 9); 
        return 1;
      } 
      unread_character(buf);
    }
    if (c != '\0') {
      return scan_token(buf, token); 
    }
  }

  switch(c) {
    case ':':
      if (read_character(buf) == '=') {
        token_init(token, ASSIGN, ":=", 2);
        return 1;
      } 
      unread_character(buf);
    case '+':
      token_init(token, PLUS, "+", 1);
      return 1;
    case '-':
      token_init(token, MINUS, "-", 1);
      return 1;
    case '*':
      token_init(token, TIMES, "*", 1);
      return 1;
    case '/':
      c = read_character(buf);
      if (c == '/') {
        int len = 2;
        while ((c = read_character(buf)) && c != '\n' && c != EOF) {
          len++; 
        }
        unread_character(buf);
        token_init(token, COMMENT, *buf - len, len);
        return 1;
      } else if (c == '*' && !in_multiline_comment) {
        in_multiline_comment = true;
        return scan_token(buf, token);
      }
      unread_character(buf); 
      token_init(token, DIV, "/", 1);
      return 1;
    case '(':
      token_init(token, LPAREN, "(", 1);
      return 1;
    case ')':
      token_init(token, RPAREN, ")", 1);
      return 1;
    case ' ':
    case '\t':
    case '\n':
      return scan_token(buf, token);
    case '\0':
    case EOF:
      return 0;
  }
  // try and parse an identifer or keyword
  if (isalpha(c)) {
    // check if keyword 
    char *keywords[] = {"read", "write"};
    int keywords_len = 2;
    for (int i = 0; i < keywords_len; i++) {
      if (strncmp(*buf - 1, keywords[i], strlen(keywords[i])) == 0) {
        *buf += strlen(keywords[i]) - 1; 
        token_init(token, KEYWORD, keywords[i], strlen(keywords[i]));
        return 1;
      }
    }

    // parse as identifer
    int len = 1;
    while ((c = read_character(buf)) && isalnum(c)) {
      len++; 
    }
    // unread the character that broke the while loop
    unread_character(buf);
    token_init(token, IDENTIFIER, *buf - len, len);
    return 1;
  }

  if (isdigit(c) || c == '.') {
    // parse as digit
    int len = 1;
    int dot_found = (c == '.');
    while ((c = read_character(buf))) {
      if (c == '.') {
        if (dot_found) {
          break;
        } 
        dot_found = true;
      } else if (!isdigit(c)) {
        break;
      } 
      len++;
    }  
    // unread the character that broke the while loop
    unread_character(buf);
    token_init(token, NUMBER, *buf - len, len);
    return 1;
  }

  char debug_str[2] = {c, '\0'};
  token_init(token, INVALID, debug_str, 1);
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s file\n", argv[0]);
    exit(1);
  }

  char *filename = argv[1];
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    printf("Can't open input file %s!\n", filename);
    exit(1);
  }

  int current_line = 0;
  int more_lines = true;
  while (more_lines) {
    int line_len = get_current_line_length(fp);
    {
      char *buf = malloc(line_len);
      char *starting_buf = buf;
      more_lines = read_line(fp, buf);
      current_line++;

      while (1) {
        Token *token = token_alloc();
        int successful_scan = scan_token(&buf, token);
        if (!successful_scan) {
          break; 
        } 
        if (token->type == INVALID) {
          printf("Error: Unrecognized token '%s' in %s (row %i, col %i)\n", token->text, filename, current_line, (int)(buf - starting_buf)); 
          exit(1);
        }
        token_print(*token); 
        token_free(token);
      }
      free(buf - line_len);
    }
  }
}
