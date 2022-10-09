#pragma once

#include "ast.h"

typedef enum statementType_t {
    STATEMENT_RETURN,
    STATEMENT_IF,
    STATEMENT_WHILE,
    STATEMENT_UNKNOWN,
} statementType_t;

char* stype_to_str(statementType_t stype);
statementType_t str_to_stype(char* str);

typedef struct astStatement_t astStatement_t;
struct astStatement_t {
    ast_t base;

    statementType_t stype;
};

void AstStatement(astStatement_t* self, statementType_t stype);
astStatement_t* newAstStatement(statementType_t stype);