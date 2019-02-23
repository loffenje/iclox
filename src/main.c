#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include "utils.h"

#define DEBUG

bool hadError = false;

typedef void * Object_t;

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
  Object_t literal;
  int line;
} Token;


typedef struct Scanner_t {
    char *source;
    vect *tokens;
    int start;
    int current;
    int line;
} Scanner;

static Scanner *scanner;

static map *keywords;

static void report(const char *fmt, ...) 
{
    va_list args;
    va_start(args, fmt);

    while (*fmt != '\0') {

        if (*fmt == 'l') {
            int line = va_arg(args, int);
            printf("Line: %d\n", line);
        } else if (*fmt == 'x') {
            char *ux = va_arg(args, char*);
            printf(" Error: Unexpected %s in argument list", ux);
        } else if (*fmt == 't') {
            char token = va_arg(args, int);
            printf("Error: Unexpected token %c", token);
        }

        ++fmt;
    }

    va_end(args);
}

void init_keywords()
{
    keywords = map_new();

    map_puti(keywords, "and",    AND);                       
    map_puti(keywords, "class",  CLASS);                     
    map_puti(keywords, "else",   ELSE);                      
    map_puti(keywords, "false",  FALSE);                     
    map_puti(keywords, "for",    FOR);                       
    map_puti(keywords, "fun",    FUN);                       
    map_puti(keywords, "if",     IF);                        
    map_puti(keywords, "nil",    NIL);                       
    map_puti(keywords, "or",     OR);                        
    map_puti(keywords, "print",  PRINT);                     
    map_puti(keywords, "return", RETURN);                    
    map_puti(keywords, "super",  SUPER);                     
    map_puti(keywords, "this",   THIS);                      
    map_puti(keywords, "true",   TRUE);                      
    map_puti(keywords, "var",    VAR);                       
    map_puti(keywords, "while",  WHILE);    
}

char *substring(int start, int end)
{
    char *substr = xmalloc(start + end + 1);

    memcpy(substr, &scanner->source[start], end);
    substr[end] = '\0';

    return substr;
}

static void run(char *source)
{
    // scanner has to process source here
    
    printf("%s\n", source);
}

static void run_file(char *source)
{
    FILE *stream;
    char *buffer;
    long flen;
    ssize_t nr;

    stream = fopen(source, "rb");
    if (!stream) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fseek(stream, 0, SEEK_END);
    flen = ftell(stream);
    rewind(stream);

    buffer = xmalloc((flen + 1) * sizeof(char));
    nr = fread(buffer, flen, 1, stream);
    if (!nr) {
        perror("fread");
        exit(EXIT_FAILURE);
    }

    run(buffer);

    fclose(stream);

    if (hadError) exit(EXIT_FAILURE);
}

static ssize_t read_line(char **source)
{
    size_t len = 0;
    ssize_t line = getline(source, &len, stdin);

    return line;
}

void run_prompt()
{
    do {
        printf("> ");
        char *line = NULL;
        read_line(&line);
        run(line);
        hadError = false;
    } while(true);
}

Scanner *new_scanner()
{
    scanner = xmalloc(sizeof(Scanner));
    scanner->tokens = vect_alloc(sizeof(Token));
    scanner->start = scanner->current = 0;
    scanner->line = 1;

    return scanner;
}

Token *new_token(TokenType type, char *lexeme, Object_t literal, int line)
{    
    Token *token;
    token = xmalloc(sizeof(Token));
    token->type = type;
    token->lexeme = strdup(lexeme);
    token->literal = literal;
    token->line = line;

    free(lexeme);

    return token;
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

void add__token(TokenType type, Object_t literal)
{
    char *substr = substring(scanner->start, scanner->current);
    Token *token = new_token(type, substr, literal, scanner->line);

    vect_push_back(scanner->tokens, token);
}

void add_token(TokenType type)
{
    add__token(type, NULL);
}

void add_token2(TokenType type, Object_t literal)
{
    add__token(type, literal);
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

void identifier()
{
    while (is_alpha_numeric(peek())) next_token();

    char *substr = substring(scanner->start, scanner->current);
    TokenType type = (intptr_t)map_get(keywords, substr);
    if (type == 0) {
        add_token(IDENTIFIER);
    } else {
        printf("ID %d\n", type);
        add_token(type);
    }

    free(substr);
}

void number() 
{
    while (is_digit(peek())) next_token();

    if (peek() == '.' && is_digit(peek_forward())) {
        next_token();

        while (is_digit(peek())) next_token();
    }

    char *number = substring(scanner->start + 1, scanner->current - 1);
    double parsed_num = atof(number);
    free(number);

    add_token2(NUMBER, &parsed_num);
}


void string()
{
    while (peek() != '\"' && !is_at_end()) {
        if (peek() == '\n') scanner->line++;
        next_token();
    }

    if (is_at_end()) {
        perror("Unterminated string\n");
        return;
    }

    next_token();

    char *substr = substring(scanner->start, scanner->current);

    add_token2(STRING, substr);
}

void scan_token()
{
    char c = next_token();
    switch(c) {
      case '(': add_token(LEFT_PAREN); break;     
      case ')': add_token(RIGHT_PAREN); break;    
      case '{': add_token(LEFT_BRACE); break;     
      case '}': add_token(RIGHT_BRACE); break;    
      case ',': add_token(COMMA); break;          
      case '.': add_token(DOT); break;            
      case '-': add_token(MINUS); break;          
      case '+': add_token(PLUS); break;           
      case ';': add_token(SEMICOLON); break;      
      case '*': add_token(STAR); break; 
      case '!': add_token(match('=') ? BANG_EQUAL : EQUAL); break;
      case '=': add_token(match('=') ? EQUAL_EQUAL : EQUAL); break;
      case '<': add_token(match('=') ? LESS_EQUAL : LESS); break;
      case '>': add_token(match('=') ? GREATER_EQUAL : GREATER); break;
      case '\"': string(); break;
      case '/': 
        if (match('/')) {
            while (peek() != '\n' && !is_at_end()) next_token();
        } else {
            add_token(SLASH); 
        }
      case ' ':
      case '\r':
      case '\t':
      case '\n':
        scanner->line++;
        break;   
      default: {
          if (is_digit(c)) {
              number();
          } else if (is_alpha(c)) {
              identifier();
          } else {
              report("t", c);
          }
      }

      break;
    }
}

vect *scan_tokens()
{
    while (!is_at_end()) {
            scanner->start = scanner->current;
            scan_token();
    }

    Token *token;
    token = xmalloc(sizeof(Token));
    token->type = EOF;
    token->lexeme = "";
    token->literal = NULL;
    token->line = scanner->line;

    vect_push_back(scanner->tokens, token);

    return scanner->tokens;
}

void test_scanner()
{
    init_keywords();
    scanner = new_scanner();
    assert(scanner);
    scanner->source = "if(1){return true;}";
    vect *tokens = scan_tokens();
    for (int i = 0; i < vect_len(tokens); i++) {
        Token *token = (Token *)vect_pop_back(tokens);
        printf("Iter %s\n", token->lexeme);
    }

    printf("Assertion ok\n");
}

int main(int argc, char **argv) 
{
    if (argc > 2) {
        printf("Usage: lox [script]");
        #ifdef DEBUG
        test_scanner();
        #endif
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        run_prompt();
    }
}