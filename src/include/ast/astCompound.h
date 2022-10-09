#pragma once

#include "ast.h"
#include "../list.h"

typedef struct astCompound_t astCompound_t;
struct astCompound_t {
    ast_t base;

    list_t* children;
};

void AstCompound(astCompound_t* self);
astCompound_t* newAstCompound();