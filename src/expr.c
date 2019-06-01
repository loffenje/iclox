#include "expr.h"
#include "memory.h"
#include "token.h"

memory_pool pool;

Expr *expr_new_expr(ExprKind kind)
{    
    Expr *e = memory_pool_alloc_arena(&pool, sizeof(Expr));

    e->kind = kind;
    e->next = NULL;

    return e;
}

Expr *expr_new_unary(Token *operator, Expr *right)
{
    Expr *e = expr_new_expr(UNARY);
    e->unary.operator = operator;
    e->unary.right = right;

    return e;
}

Expr *expr_new_literal(Token *value)
{
    Expr *e = expr_new_expr(LITERAL);
    e->literal.value = *value;

    return e;
}

Expr *expr_new_number_literal(double value)
{
    Expr *e = expr_new_expr(LITERAL);
    e->literal.value.number = value;
    e->literal.value.type = NUMBER;
    e->literal.value.literal = NULL;

    return e;
}

Expr *expr_new_logical(Expr *left, Token *operator, Expr *right)
{
    Expr *e = expr_new_expr(LOGICAL);
    e->logical.left = left;
    e->logical.operator = operator;
    e->logical.right = right;

    return e;
}

Expr *expr_new_assign(Token *name, Expr *value)
{
    Expr *e = expr_new_expr(ASSIGN);
    e->assign.name = name;
    e->assign.value = value;

    return e;
}

Expr *expr_new_binary(Expr *left, Token *operator, Expr *right)
{
    Expr *e = expr_new_expr(BINARY);
    e->binary.left = left;
    e->binary.operator = operator;
    e->binary.right = right;

    return e;
}

Expr *expr_new_call(Expr *callee, Token *paren, Expr *arguments)
{
    Expr *e = expr_new_expr(CALL);
    e->call.callee = callee;
    e->call.paren = paren;
    e->call.arguments = arguments;

    return e;
}

Expr *expr_new_grouping(Expr *expr)
{
    Expr *e = expr_new_expr(GROUPING);
    e->grouping.expression = expr;

    return e;
}

Expr *test_expr()
{
    Expr *e = expr_new_binary(
        expr_new_unary(
            token_new_atomic_token(MINUS, "-", 1),
            expr_new_number_literal(123)
        ),
        token_new_atomic_token(STAR, "*", 1),
        expr_new_grouping(expr_new_number_literal(12.33))
    );

    return e;
}
