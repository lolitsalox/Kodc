#include "../include/ast/astStatement.h"
#include <string.h>

char* stype_to_str(statementType_t stype) {
    switch (stype) {
        case STATEMENT_RETURN:          return "STATEMENT_RETURN";
        case STATEMENT_IF:              return "STATEMENT_IF";
        case STATEMENT_WHILE:           return "STATEMENT_WHILE";
        default:                        return "STATEMENT_UNKNOWN";
    }
}

statementType_t str_to_stype(char* str) {
    if (strcmp(str, "return") == 0)     return STATEMENT_RETURN;
    if (strcmp(str, "if") == 0)         return STATEMENT_IF;
    if (strcmp(str, "while") == 0)      return STATEMENT_WHILE;
    return STATEMENT_UNKNOWN;
}

void AstStatement(astStatement_t* self, statementType_t stype) {
    Ast(&self->base, AST_STATEMENT);
    
    self->stype = stype;
}

astStatement_t* newAstStatement(statementType_t stype) {
    astStatement_t* self = (astStatement_t*) malloc(sizeof(astStatement_t));
    if (!self) error_malloc(__func__);

    AstStatement(self, stype);
    return self;
}