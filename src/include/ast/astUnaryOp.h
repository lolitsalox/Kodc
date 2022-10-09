#pragma once

#include "ast.h"
#include "../token.h"

typedef struct astUnaryOp_t astUnaryOp_t;
struct astUnaryOp_t {
    ast_t base;

    tokenType_t op;
    ast_t* value;
};

void AstUnaryOp(astUnaryOp_t* self, tokenType_t op, ast_t* value);
astUnaryOp_t* newAstUnaryOp(tokenType_t op, ast_t* value);