#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct {
  TokenType type;
  char *text;
} Token;

Token *token_new(TokenType type, char *text, int len);
void token_free(Token *token);
void token_print(Token token);
Token *token_alloc(void);
void token_init(Token *token, TokenType type, char *text, int len);
void token_deinit(Token *token);

#ifdef __cplusplus
}
#endif
