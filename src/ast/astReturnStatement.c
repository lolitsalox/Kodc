#include "../include/ast/astReturnStatement.h"

void AstReturnStatement(astReturnStatement_t* self, ast_t* value) {
    AstStatement(&self->base, STATEMENT_RETURN);
    
    self->value = value;
}

astReturnStatement_t* newAstReturnStatement(ast_t* value) {
    astReturnStatement_t* self = (astReturnStatement_t*) malloc(sizeof(astReturnStatement_t));
    if (!self) error_malloc(__func__);

    AstReturnStatement(self, value);
    return self;
}