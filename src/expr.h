#ifndef EXPR_H
#define EXPR_H

#include <stdbool.h>

#include "token.h"

typedef struct Expr Expr;

typedef enum ExprKind {
    ASSIGN,
    BINARY,
    CALL,
    UNARY,
    VARIABLE,
    LOGICAL,
    LITERAL,
    GET,
    SET,
    GROUPING
} ExprKind;

typedef struct ExprTag {
    ExprKind kind;
    struct ExprTag *next;
} ExprTag;

struct Expr {
    ExprKind kind;
    Expr *next;
    union {
        struct {
            Token *name;
            Expr *value;
        } assign;

        struct {
            Expr *left;
            Token *operator;
            Expr *right;
        } binary;

        struct {
            Expr *callee;
            Token *paren;
            Expr *arguments;
        } call;

        struct {
            Token value;
        } literal;

        struct {
            Token *operator;
            Expr *right;
        } unary;

        struct {
            Token *name;
        } variable;

        struct {
            Expr *expression;
        } grouping;

        struct {
            Expr  *left;
            Token *operator;
            Expr  *right;
        } logical;
    };

};

Expr *expr_new_expr(ExprKind kind);
Expr *expr_new_unary(Token *operator, Expr *right);
Expr *expr_new_literal(Token *value);
Expr *expr_new_number_literal(double value);
Expr *expr_new_logical(Expr *left, Token *operator, Expr *right);
Expr *expr_new_assign(Token *name, Expr *value);
Expr *expr_new_binary(Expr *left, Token *operator, Expr *right);
Expr *expr_new_call(Expr *callee, Token *paren, Expr *arguments);
Expr *expr_new_grouping(Expr *expr);

Expr *test_expr();

#endif
