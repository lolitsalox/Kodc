#pragma once

#include "ast.h"

typedef struct astVariable_t astVariable_t;
struct astVariable_t {
    ast_t base;

    char* name;
    int stackIndex;
};

void AstVariable(astVariable_t* self, char* name);
astVariable_t* newAstVariable(char* name);