#include <string.h>

#include "token.h"

Token *token_new_token(TokenType type, char *lexeme, Object literal, int line)
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

Token *token_new_bool_token(TokenType type, char *lexeme, bool literal, int line) 
{
    Token *token;
    token = xmalloc(sizeof(Token));

    token->type = type;
    token->lexeme = lexeme;
    token->literal = &literal;
    token->line = line;

    return token;
}

Token *token_new_atomic_token(TokenType type, char *lexeme, int line)
{    
    Token *token;
    token = xmalloc(sizeof(Token));

    token->type = type;
    token->lexeme = strdup(lexeme);
    token->literal = NULL;
    token->line = line;

    return token;
}

