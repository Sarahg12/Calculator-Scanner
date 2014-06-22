#include "src/calculator_scanner.h"
#include "src/token.h"

TokenType get_first_token_type(char *text) {
  Token *token = scan_string(&text);
  return token->type;
}
