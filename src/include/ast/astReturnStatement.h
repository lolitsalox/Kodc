#pragma once

#include "astStatement.h"

typedef struct astReturnStatement_t astReturnStatement_t;
struct astReturnStatement_t {
    astStatement_t base;

    ast_t* value;
};

void AstReturnStatement(astReturnStatement_t* self, ast_t* value);
astReturnStatement_t* newAstReturnStatement(ast_t* value);