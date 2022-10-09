#pragma once

#include "astCompound.h"

typedef struct astCall_t astCall_t;
struct astCall_t {
    ast_t base;

    char* name;
    astCompound_t* arguments;
};

void AstCall(astCall_t* self, char* name, astCompound_t* arguments);
astCall_t* newAstCall(char* name, astCompound_t* arguments);