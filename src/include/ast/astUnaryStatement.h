#pragma once

#include "astStatement.h"

typedef struct astUnaryStatement_t astUnaryStatement_t;
struct astUnaryStatement_t {
    astStatement_t base;

    ast_t* value;
};

void AstUnaryStatement(astUnaryStatement_t* self, statementType_t stype,  ast_t* value);
astUnaryStatement_t* newAstUnaryStatement(statementType_t stype, ast_t* value);