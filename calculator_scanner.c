#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define WTF printf("WTF"); exit(1)

int in_multiline_comment;

typedef enum {
  ASSIGN,
  PLUS,
  MINUS,
  TIMES,
  DIV,
  LPAREN,
  RPAREN,
  IDENTIFIER,
  KEYWORD,
  NUMBER,
  COMMENT,
  INVALID,
} TokenType;

char *get_token_type_name(TokenType type) {
  switch (type) {
    case ASSIGN: return "ASSIGN";
    case PLUS: return "PLUS";
    case MINUS: return "MINUS";
    case TIMES: return "TIMES";
    case DIV: return "DIV"; 
    case LPAREN: return "LPAREN";
    case RPAREN: return "RPAREN";
    case IDENTIFIER: return "IDENTIFIER";
    case KEYWORD: return "KEYWORD";
    case NUMBER: return "NUMBER"; 
    case COMMENT: return "COMMENT"; 
    case INVALID: return "INVALID";
  }
  WTF;
}

typedef struct {
  TokenType type;
  char *text;
} Token;

void print_error_and_exit(void) {
  printf("Error: invalid input\n");
  exit(1);
}

char read_character(char **str) {
  char c = **str;
  (*str)++;
  return c; 
}

Token *token_new(TokenType type, char *text, int len) {
  Token *token = malloc(sizeof(Token));
  token->type = type;
  token->text = malloc(len + 1);
  strncpy(token->text, text, len);
  token->text[len] = '\0';
  return token;
}

void token_free(Token *token) {
  free(token->text);
  free(token);
}

void unread_character(char **str) {
  (*str)--;
}
int scan_token(char **buf, Token **token) {
  char c = read_character(buf);

  if (in_multiline_comment) {
    if (c == '*') {
      if (read_character(buf) == '/') {
        in_multiline_comment = false;
        *token = token_new(COMMENT, "/* ... */", 9); 
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
        *token = token_new(ASSIGN, ":=", 2);
        return 1;
      } 
      *token = token_new(INVALID, "", 0);
      return 1;
    case '+':
      *token = token_new(PLUS, "+", 1);
      return 1;
    case '-':
      *token = token_new(MINUS, "-", 1);
      return 1;
    case '*':
      *token = token_new(TIMES, "*", 1);
      return 1;
    case '/':
      c = read_character(buf);
      if (c == '/') {
        int len = 2;
        while ((c = read_character(buf)) && c != '\n' && c != EOF) {
          len++; 
        }
        unread_character(buf);
        *token = token_new(COMMENT, *buf - len, len);
        return 1;
      } else if (c == '*' && !in_multiline_comment) {
        in_multiline_comment = true;
        return scan_token(buf, token);
      }
      unread_character(buf); 
      *token = token_new(DIV, "/", 1);
      return 1;
    case '(':
      *token = token_new(LPAREN, "(", 1);
      return 1;
    case ')':
      *token = token_new(RPAREN, ")", 1);
      return 1;
    case ' ':
    case '\t':
    case '\n':
      return scan_token(buf, token);
  }
  // try and parse an identifer or keyword
  if (isalpha(c)) {
    // check if keyword 
    char *keywords[] = {"read", "write"};
    int keywords_len = 2;
    for (int i = 0; i < keywords_len; i++) {
      if (strcmp(*buf, keywords[i]) == 0) {
        *buf += strlen(keywords[i]) - 1; 
        *token = token_new(KEYWORD, keywords[i], strlen(keywords[i]));
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
    *token = token_new(IDENTIFIER, *buf - len, len);
    return 1;
  }

  if (isdigit(c)) {
    // parse as digit
    int len = 1;
    int dot_found = false;
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
    *token = token_new(NUMBER, *buf - len, len);
    return 1;
  }

  return 0;
}

void print_token(Token token) {
  char *token_type_name = get_token_type_name(token.type);
  printf("Token %s, type: %s\n", token.text, token_type_name); 
}

// char *buf must be large enough to hold the entire line
int read_line(FILE *fp, char *buf) {
  int i = 0;
  while (1) {
    char c = fgetc(fp);
    if (c == '\n' || c == EOF) {
      buf[i] = '\0';
      return c == '\n';
    }
    buf[i++] = c;
  } 
  
  WTF;
  return 0;
}

int get_current_line_length(FILE *fp) {
  int len = 0;
  char c;
  while((c = fgetc(fp)) && c != '\n' && c != EOF) {
    len++; 
  }

  // unread the read characters
  if (c == EOF) {
    fseek(fp, -len, SEEK_CUR);
  } else {
    fseek(fp, -len - 1, SEEK_CUR);
  }

  // add room for null-delimiter
  return len + 1;
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

  int more_lines = true;
  while (more_lines) {
    int line_len = get_current_line_length(fp);
    char *buf = malloc(line_len);
    {
      more_lines = read_line(fp, buf);

      Token *token;
      while (scan_token(&buf, &token)) {
        if (token->type == INVALID) {
          printf("Error: bad token\n"); 
          exit(1);
        }
        print_token(*token); 
      }
    }
    free(buf - line_len);
  }
}
