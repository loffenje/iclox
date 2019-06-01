#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include "expr.h"

typedef struct Parser_t {
    vect *tokens;
    int32_t current;
} Parser;

extern Parser *parser;

Expr *parser_parse();
Parser *parser_new_parser();

#endif