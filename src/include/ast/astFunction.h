#pragma once

#include "astCompound.h"
#include "../scope.h"

typedef struct astFunction_t astFunction_t;
struct astFunction_t {
    astCompound_t base;

    char* name;
    astCompound_t* body;
    scope_t* scope;
};

void AstFunction(astFunction_t* self);
astFunction_t* newAstFunction();