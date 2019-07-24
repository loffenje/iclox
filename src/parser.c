#include <stdarg.h>
#include <stdbool.h>


#include "parser.h"
#include "expr.h"
#include "token.h"
#include "utils.h"

Parser *parser;

static Expr *expression();

Parser *parser_new_parser(vect *tokens)
{
   parser = xmalloc(sizeof(Parser));
   parser->tokens = tokens;
   parser->current = 0;

   return parser;
}

static inline Token *peek()
{
    return vect_at(parser->tokens, parser->current);
}

static inline Token *previous()
{
    return vect_at(parser->tokens, parser->current - 1);
}

static inline bool is_at_end()
{
    Token *token = peek();

    return token->type == _EOF;
}

static inline Token *advance() 
{
    if (!is_at_end()) parser->current++;

    return previous();
}

static inline bool check(TokenType t) 
{
    if (!is_at_end()) parser->current++;
    Token *token = previous();
    if (token) {
        return token->type == t;
    } 

    return false;
}

static bool match(int n, ...) 
{
    va_list l;
    va_start(l, n);
    for (int i = 0; i < n; i++) {
        TokenType t = (TokenType) n;
    	if (check(t)) {
            advance();

            return true;
        }
    }

    return false;
}

static void syntax_error(Token *token, const char *message) 
{
    if (token->type == _EOF) {
        size_t buf_len = strlen(message) + 7;
        char msg[buf_len];
        snprintf(msg, buf_len, "at end %s", message);
        report("ls", token->line, msg);
    } else {
        size_t buf_len = strlen(message) + strlen(token->lexeme) + 4;
        char msg[buf_len];
        snprintf(msg, buf_len, "at '%s' %s",token->lexeme, message);
        report("ls", token->line, msg);
    }

    die("runtime error");
}

static inline Token *consume(TokenType type, char *message)
{
    if (check(type)) return advance();
    
    syntax_error(peek(), message);

    return NULL;
}

// primary -> NUMBER | STRING | "false" | "true" | "nil" | "(" expression ")"
static Expr *primary()
{

    if (match(FALSE)) {
         Token *token = token_new_bool_token(FALSE, "false", false, 0);
         
	 return expr_new_literal(token);
    } 

    if (match(TRUE)) {
         Token *token = token_new_bool_token(TRUE, "true", true, 0);
         
	 return expr_new_literal(token);
    } 

    if (match(NIL)) return expr_new_literal(NULL);

    if (match(NUMBER, STRING)) {
        Token *t = previous();
        return expr_new_literal(t->literal);
    }

    if (match(LEFT_PARENT)) {
        Expr *expr = expression();
        consume(RIGHT_PARENT, "Expect ')' after expression.");

        return expr_new_grouping(expr);
    }

    syntax_error(peek(), "Expect expression.");

    return NULL;
}

// unary -> ("!" | "-") unary 
//          | primary

static Expr *unary()
{
    if (match(BANG, MINUS)) {
        Token *operator = previous();
        Expr *right = unary();
        
        return expr_new_unary(operator, right);
    }

    return primary();
}


// multiplication -> unary (( "/" | "*") unary)*
static Expr *multiplication()
{
    Expr *expr = unary();

    while (match(SLASH, STAR)) {
        Token *operator = previous();
        Expr *right = unary();
        expr = expr_new_binary(expr, operator, right);
    }

    return expr;
}

// addition -> multiplication ( ("-" | "+") multiplication)*
static Expr *addition()
{
    Expr *expr = multiplication();

    while (match(MINUS, PLUS)) {
        Token *operator = previous();
        Expr *right = multiplication();
        expr = expr_new_binary(expr, operator, right);
    }

    return expr;
}

// comparisson -> addition ( (">" | ">=" | "<" | "<=") addition)*
static Expr *comparisson()
{
    Expr *expr = addition();

    while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
        Token *operator = previous();
        Expr *right = addition();
        expr = expr_new_binary(expr, operator, right);
    }

    return expr;
}

// equality -> comparison ( ( "!=" | "==") comparison ) *
static Expr *equality()
{
    Expr *expr = comparisson();

    while (match(BANG_EQUAL, EQUAL_EQUAL)) {
   	Token *operator = previous(); 
        Expr *right = comparisson();
        expr = expr_new_binary(expr, operator, right);
    }

    return expr;
}

static Expr *expression() 
{
    return equality();
}

Expr *parser_parse()
{
    return expression();
}


