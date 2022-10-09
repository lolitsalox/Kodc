#pragma once

#include "lexer.h"
#include "ast/ast.h"

typedef struct parser_t parser_t;

struct parser_t {
    lexer_t* lexer;
    token_t* token;

    ast_t* (*Parse) (parser_t* self);
};

void Parser(parser_t* self, lexer_t* lexer);
parser_t* newParser(lexer_t* lexer);