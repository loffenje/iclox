#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include "scanner.h"
#include "utils.h"

#define DEBUG

bool hadError = false;

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
        die("fopen");
    }

    fseek(stream, 0, SEEK_END);
    flen = ftell(stream);
    rewind(stream);

    buffer = xmalloc((flen + 1) * sizeof(char));
    nr = fread(buffer, flen, 1, stream);
    if (!nr) {
        die("fread");
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

/**** temporary test section ****/
void test_scanner()
{
    init_keywords();
    scanner = new_scanner();
    assert(scanner);
    scanner->source = "if(1){print(\"d\");return true;}";
    vect *tokens = scanner_scan_tokens();
    for (int i = 0; i < vect_len(tokens); i++) {
        Token *token = (Token *)vect_pop_back(tokens);
        printf("Lexeme: %s\n", token->lexeme);
    }

    printf("Assertion ok\n");
}

// end test


int main(int argc, char **argv) 
{
    if (argc > 2) {
        printf("Usage: lox [script]\n");
        #ifdef DEBUG
        test_scanner();
        #endif
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        run_prompt();
    }
}