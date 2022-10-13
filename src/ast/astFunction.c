#include "../include/ast/astFunction.h"

void FunctionPrint(astFunction_t* self, size_t indent);

void AstFunction(astFunction_t* self) {
    AstCompound(&self->base);
    self->base.base.Print = (void (*)(ast_t*, size_t)) FunctionPrint;
    self->base.base.type = AST_FUNCTION;
    self->base.base.dtypeInfo.dtype = DTYPE_U64;
    
    self->name = "<anonymous>";
    self->body = newAstCompound();
    self->base.base.dtypeReturnInfo.dtype    = DTYPE_UNKNOWN;
    self->base.base.dtypeReturnInfo.ptrCount = 0;

    self->scope = NULL;
}

astFunction_t* newAstFunction() {
    astFunction_t* self = (astFunction_t*) malloc(sizeof(astFunction_t));
    if (!self) error_malloc(__func__);

    AstFunction(self);
    return self;
}

void FunctionPrint(astFunction_t* self, size_t indent) {
    INDENT(indent)
    printf("%s\n", astTypeToStr(self->base.base.type));
    
    INDENT(indent + 1)
    printf("name: %s\n", self->name);
    
    INDENT(indent + 1)
    printf("return type: ");

    for (size_t i = 0; i < self->base.base.dtypeReturnInfo.ptrCount; ++i)
        printf("#");
    printf("%s\n", dTypeToStr(self->base.base.dtypeReturnInfo.dtype));
    
    INDENT(indent + 1)
    printf("Parameters:\n");

    for (size_t i = 0; i < self->base.children->size; ++i) {
        // INDENT(indent + 1)
        // printf("Parameter #%lld:\n", i);
        ast_t* child = (ast_t*) self->base.children->items[i];
        child->Print(child, indent + 2);
    }

    ((ast_t*)self->body)->Print((ast_t*)self->body, indent + 1);
}