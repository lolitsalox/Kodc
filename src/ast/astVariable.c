#include "../include/ast/astVariable.h"

void VariablePrint(astVariable_t* self, size_t indent);

void AstVariable(astVariable_t* self, char* name) {
    Ast(&self->base, AST_VARIABLE);
    self->base.Print = (void (*)(ast_t*, size_t)) VariablePrint;

    self->name = name;
    self->stackIndex = 0;
    self->typeDefined = false;
}

astVariable_t* newAstVariable(char* name) {
    astVariable_t* self = (astVariable_t*) malloc(sizeof(astVariable_t));
    if (!self) error_malloc(__func__);

    AstVariable(self, name);
    return self;
}

void VariablePrint(astVariable_t* self, size_t indent) {
    INDENT(indent)
    printf("%s\n", astTypeToStr(self->base.type));
    
    INDENT(indent + 1)
    printf("name: %s\n", self->name);

    INDENT(indent + 1);
    printf("type: ");
    for (size_t i = 0; i < self->base.dtypeInfo.ptrCount; ++i)
        printf("#");
    printf("%s\n", dTypeToStr(self->base.dtypeInfo.dtype));
}