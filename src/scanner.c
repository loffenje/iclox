#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "token.h"

Scanner *scanner;

map *keywords;

Scanner *scanner_new_scanner()
{
    scanner = xmalloc(sizeof(Scanner));
    scanner->tokens = vect_alloc(sizeof(Token));
    scanner->start = 0;
    scanner->current = 0;
    scanner->line = 1;

    return scanner;
}

static inline bool is_at_end()
{
    return scanner->current >= strlen(scanner->source);
}

static inline char next_token()
{
    scanner->current++;
    return scanner->source[scanner->current - 1];
}

void scanner_add__token(TokenType type, Object literal)
{
    char *substr = scanner_substring(scanner->start, scanner->current);
    Token *token = token_new_token(type, substr, literal, scanner->line);

    vect_push_back(scanner->tokens, token);
}

void scanner_add_token(TokenType type)
{
    scanner_add__token(type, NULL);
}

void scanner_add_token2(TokenType type, Object literal)
{
    scanner_add__token(type, literal);
}

static inline bool match(char expected)
{
    if (is_at_end()) return false;

    if (scanner->source[scanner->current] == expected) {
        scanner->current++;
        return true;
    }

    return false;
}

static inline bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

static inline char peek()
{
    if (is_at_end()) return '\0';

    return scanner->source[scanner->current];
}

static inline char peek_forward()
{
    if (scanner->current + 1 >= strlen(scanner->source)) return '\0';

    return scanner->source[scanner->current + 1];
}

static inline bool is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c == '_');
}

static inline bool is_alpha_numeric(char c)
{
    return is_alpha(c) || is_digit(c);
}

void scanner_identifier()
{
    while (is_alpha_numeric(peek())) next_token();

    char *substr = scanner_substring(scanner->start, scanner->current);
    TokenType type = (intptr_t)map_get(keywords, substr);
    if (type == 0) {
        scanner_add_token(IDENTIFIER);
    } else {
        scanner_add_token(type);
    }

    free(substr);
}

void scanner_number() 
{
    while (is_digit(peek())) next_token();

    if (peek() == '.' && is_digit(peek_forward())) {
        next_token();

        while (is_digit(peek())) next_token();
    }

    char *number = scanner_substring(scanner->start + 1, scanner->current - 1);
    double parsed_num = atof(number);
    free(number);

    scanner_add_token2(NUMBER, &parsed_num);
}


void scanner_string()
{
    while (peek() != '\"' && !is_at_end()) {
        if (peek() == '\n') scanner->line++;
        next_token();
    }

    if (is_at_end()) {
        die("Unterminated string\n");
        return;
    }

    next_token();

    char *substr = scanner_substring(scanner->start, scanner->current);

    scanner_add_token2(STRING, substr);
}

void scanner_scan_token()
{
    char c = next_token();
    switch(c) {
      case '(': scanner_add_token(LEFT_PARENT); break;     
      case ')': scanner_add_token(RIGHT_PARENT); break;    
      case '{': scanner_add_token(LEFT_BRACE); break;     
      case '}': scanner_add_token(RIGHT_BRACE); break;    
      case ',': scanner_add_token(COMMA); break;          
      case '.': scanner_add_token(DOT); break;            
      case '-': scanner_add_token(MINUS); break;          
      case '+': scanner_add_token(PLUS); break;           
      case ';': scanner_add_token(SEMICOLON); break;      
      case '*': scanner_add_token(STAR); break; 
      case '!': scanner_add_token(match('=') ? BANG_EQUAL : EQUAL); break;
      case '=': scanner_add_token(match('=') ? EQUAL_EQUAL : EQUAL); break;
      case '<': scanner_add_token(match('=') ? LESS_EQUAL : LESS); break;
      case '>': scanner_add_token(match('=') ? GREATER_EQUAL : GREATER); break;
      case '\"': scanner_string(); break;
      case '/': 
        if (match('/')) {
            while (peek() != '\n' && !is_at_end()) next_token();
        } else {
            scanner_add_token(SLASH); 
        }
      case ' ':
      case '\r':
      case '\t':
      case '\n':
        scanner->line++;
        break;   
      default: {
          if (is_digit(c)) {
              scanner_number();
          } else if (is_alpha(c)) {
              scanner_identifier();
          } else {
              report("t", c);
          }
      }

      break;
    }
}

vect *scanner_scan_tokens()
{
    while (!is_at_end()) {
            scanner->start = scanner->current;
            scanner_scan_token();
    }

    Token *token;
    token = xmalloc(sizeof(Token));
    token->type = _EOF;
    token->lexeme = "";
    token->literal = NULL;
    token->line = scanner->line;

    vect_push_back(scanner->tokens, token);

    return scanner->tokens;
}

char *scanner_substring(int start, int end)
{
    char *substr = xmalloc(start + end + 1);

    memcpy(substr, &scanner->source[start], end);
    substr[end] = '\0';

    return substr;
}
