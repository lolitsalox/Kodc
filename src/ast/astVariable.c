#include "../include/ast/astVariable.h"

void AstVariable(astVariable_t* self, char* name) {
    Ast(&self->base, AST_VARIABLE);
    // self->base.Print = (void (*)(ast_t*, size_t)) VariablePrint;

    self->name = name;
    self->stackIndex = 0;
}

astVariable_t* newAstVariable(char* name) {
    astVariable_t* self = (astVariable_t*) malloc(sizeof(astVariable_t));
    if (!self) error_malloc(__func__);

    AstVariable(self, name);
    return self;
}