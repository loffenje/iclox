#ifndef SCANNER_H
#define SCANNER_H

#include "utils.h"

typedef void *Object;

typedef enum TokenType_t {
  // Single-character tokens.                      
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
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
} TokenType;

typedef struct Token_t {
  TokenType type;
  char *lexeme;
  Object literal;
  int line;
} Token;


typedef struct Scanner_t {
    char *source;
    vect *tokens;
    int start;
    int current;
    int line;
} Scanner;

extern Scanner *scanner;
extern map *keywords;

Scanner *new_scanner();
Token *new_token(TokenType type, char *lexeme, Object literal, int line);
void scanner_add__token(TokenType type, Object literal);
void scanner_add_token(TokenType type);
void scanner_add_token2(TokenType type, Object literal);
void scanner_identifier();
void scanner_number();
void scanner_string();
void scanner_scan_token();
vect *scanner_scan_tokens();
char *scanner_substring(int start, int end);

#endif