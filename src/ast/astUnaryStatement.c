#include "../include/ast/astUnaryStatement.h"

void AstUnaryStatement(astUnaryStatement_t* self, statementType_t stype, ast_t* value) {
    AstStatement(&self->base, stype);
    
    self->value = value;
}

astUnaryStatement_t* newAstUnaryStatement(statementType_t stype, ast_t* value) {
    astUnaryStatement_t* self = (astUnaryStatement_t*) malloc(sizeof(astUnaryStatement_t));
    if (!self) error_malloc(__func__);

    AstUnaryStatement(self, stype, value);
    return self;
}