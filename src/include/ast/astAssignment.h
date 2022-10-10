#pragma once

#include "astBin.h"
#include <stdbool.h>

typedef struct astAssignment_t astAssignment_t;
struct astAssignment_t {
    astBin_t base;

    bool typeDefined; // id: dtype <- this thing
};

void AstAssignment(astAssignment_t* self, bool typeDefined, ast_t* left, ast_t* right);
astAssignment_t* newAstAssignment(bool typeDefined, ast_t* left, ast_t* right);