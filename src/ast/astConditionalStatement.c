#include "../include/ast/astConditionalStatement.h"

void AstConditionalStatement(
    astConditionalStatement_t* self, 
    statementType_t stype, 
    ast_t* condition, 
    astCompound_t* body
) {
    AstStatement(&self->base, stype);
    self->base.base.type = AST_CONDITIONAL_STATEMENT;
    
    self->condition = condition;
    self->body = body;
}

astConditionalStatement_t* newAstConditionalStatement(
    statementType_t stype, 
    ast_t* condition, 
    astCompound_t* body
) {
    astConditionalStatement_t* self = (astConditionalStatement_t*) malloc(sizeof(astConditionalStatement_t));
    if (!self) error_malloc(__func__);

    AstConditionalStatement(self, stype, condition, body);
    return self;
}