#pragma once

#include "ast.h"
#include <stdbool.h>

typedef struct astVariable_t astVariable_t;
struct astVariable_t {
    ast_t base;

    char* name;
    int stackIndex;
    bool typeDefined;
};

void AstVariable(astVariable_t* self, char* name);
astVariable_t* newAstVariable(char* name);