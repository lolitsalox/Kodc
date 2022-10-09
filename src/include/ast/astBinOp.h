#pragma once

#include "astBin.h"
#include "../token.h"

typedef struct astBinOp_t astBinOp_t;
struct astBinOp_t {
    astBin_t base;

    tokenType_t op;
};

void AstBinOp(astBinOp_t* self, tokenType_t op, ast_t* left, ast_t* right);
astBinOp_t* newAstBinOp(tokenType_t op, ast_t* left, ast_t* right);