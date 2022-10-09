#pragma once

#include "astStatement.h"
#include "astCompound.h"

typedef struct astConditionalStatement_t astConditionalStatement_t;
struct astConditionalStatement_t {
    astStatement_t base;

    ast_t* condition;
    astCompound_t* body;
};

void AstConditionalStatement(
    astConditionalStatement_t* self, 
    statementType_t stype, 
    ast_t* condition, 
    astCompound_t* body
);

astConditionalStatement_t* newAstConditionalStatement(
    statementType_t stype, 
    ast_t* condition, 
    astCompound_t* body
);