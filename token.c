#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"

Token *token_new(TokenType type, char *text, int len) {
  Token *token = token_alloc();
  token_init(token, type, text, len);
  return token;
}

Token *token_alloc(void) {
  Token *token = malloc(sizeof(Token));
  return token;
}

void token_init(Token *token, TokenType type, char *text, int len) {
  token->type = type;
  token->text = malloc(len + 1);
  strncpy(token->text, text, len);
  token->text[len] = '\0';
}

void token_deinit(Token *token) {
  free(token->text);
}

void token_free(Token *token) {
  token_deinit(token);
  free(token);
}

static char *get_token_type_name(TokenType type) {
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

void token_print(Token token) {
  char *token_type_name = get_token_type_name(token.type);
  printf("Token %s, type: %s\n", token.text, token_type_name); 
}


