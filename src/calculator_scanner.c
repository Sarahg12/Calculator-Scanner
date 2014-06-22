#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "token.h"
#include "file_reader.h"

// returns a token on the heap, or a null pointer
Token *scan_string(char **buf) {
  char *starting_buf = *buf;
  static int in_multiline_comment = false;
  char c = read_character(buf);

  if (in_multiline_comment) {
    if (c == '*') {
      if (read_character(buf) == '/') {
        in_multiline_comment = false;
        return token_new(COMMENT, "/* ... */", 9); 
      } 
      unread_character(buf);
    }
    if (c != '\0') {
      return scan_string(buf); 
    }
  }

  switch(c) {
    case ':':
      if (read_character(buf) == '=') {
        return token_new(ASSIGN, ":=", 2);
      } 
      unread_character(buf);
    case '+':
      return token_new(PLUS, "+", 1);
    case '-':
      return token_new(MINUS, "-", 1);
    case '*':
      return token_new(TIMES, "*", 1);
    case '/':
      c = read_character(buf);
      if (c == '/') {
        int len = 2;
        while ((c = read_character(buf)) && c != '\n' && c != EOF) {
          len++; 
        }
        unread_character(buf);
        return token_new(COMMENT, *buf - len, len);
      } else if (c == '*' && !in_multiline_comment) {
        in_multiline_comment = true;
        return scan_string(buf);
      }
      unread_character(buf); 
      return token_new(DIV, "/", 1);
    case '(':
      return token_new(LPAREN, "(", 1);
    case ')':
      return token_new(RPAREN, ")", 1);
    case ' ':
    case '\t':
    case '\n':
      return scan_string(buf);
    case '\0':
    case EOF:
      return NULL;
  }
  // try and parse an identifer or keyword
  if (isalpha(c)) {
    // check if keyword 
    char *keywords[] = {"read", "write"};
    int keywords_len = 2;
    for (int i = 0; i < keywords_len; i++) {
      if (strncmp(*buf - 1, keywords[i], strlen(keywords[i])) == 0) {
        *buf += strlen(keywords[i]) - 1; 
        return token_new(KEYWORD, keywords[i], strlen(keywords[i]));
      }
    }

    // parse as identifer
    int len = 1;
    while ((c = read_character(buf)) && isalnum(c)) {
      len++; 
    }
    // unread the character that broke the while loop
    unread_character(buf);
    return token_new(IDENTIFIER, *buf - len, len);
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
    return token_new(NUMBER, *buf - len, len);
  }

  char debug_str[2] = {c, '\0'};
  return token_new(INVALID, debug_str, 1);
}

void scan_file(char *filename) {
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
        Token *token = scan_string(&buf);
        if (!token) {
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

