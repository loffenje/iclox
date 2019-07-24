#ifndef TOKEN_H
#define TOKEN_H

#include "utils.h"

typedef enum TokenType_t {
  // Single-character tokens.                      
  LEFT_PARENT, RIGHT_PARENT, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR, 

  // One or two character tokens.                  
  BANG, BANG_EQUAL,                                
  EQUAL, EQUAL_EQUAL,                              
  GREATER, GREATER_EQUAL,                          
  LESS, LESS_EQUAL,                                

  // Literals.                                     
  IDENTIFIER, STRING, NUMBER,                      

  // Keywords
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,  
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,      

  // EOF
  _EOF                     
} TokenType;

typedef struct Token_t {
  double number;
  TokenType type;
  char *lexeme;
  Object literal;
  int line;
} Token;

Token *token_new_token(TokenType type, char *lexeme, Object literal, int line);
Token *token_new_atomic_token(TokenType type, char *lexeme, int line);

Token *token_new_bool_token(TokenType type, char *lexeme, bool literal, int line);

#endif
