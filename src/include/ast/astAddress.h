#pragma once

#include "ast.h"
#include <stdbool.h>

typedef struct astAddress_t astAddress_t;
struct astAddress_t {
    ast_t base;

    bool ref;   // true: #, false: @
    ast_t* value;
};

void AstAddress(astAddress_t* self, bool ref, ast_t* value);
astAddress_t* newAstAddress(bool ref, ast_t* value);