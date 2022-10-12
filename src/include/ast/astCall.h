#pragma once

#include "astCompound.h"

typedef struct astCall_t astCall_t;
struct astCall_t {
    ast_t base;

    ast_t* value;
    astCompound_t* arguments;
};

void AstCall(astCall_t* self, ast_t* value, astCompound_t* arguments);
astCall_t* newAstCall(ast_t* value, astCompound_t* arguments);