#ifndef SCANNER_H
#define SCANNER_H

#include "utils.h"
#include "token.h"


typedef struct Scanner_t {
    char *source;
    vect *tokens;
    int start;
    int current;
    int line;
} Scanner;

extern Scanner *scanner;
extern map *keywords;

Scanner *scanner_new_scanner();
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