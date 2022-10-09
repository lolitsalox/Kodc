#pragma once

#include "ast.h"

typedef struct astBin_t astBin_t;
struct astBin_t {
    ast_t base;

    ast_t* left;
    ast_t* right;
};

void AstBin(astBin_t* self, astType_t type, ast_t* left, ast_t* right);
astBin_t* newAstBin(astType_t type, ast_t* left, ast_t* right);