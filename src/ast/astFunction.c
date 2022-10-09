#include "../include/ast/astFunction.h"

void AstFunction(astFunction_t* self) {
    AstCompound(&self->base);
    // self->base.base.Print = (void (*)(ast_t*, size_t)) FunctionPrint;
    self->base.base.type = AST_FUNCTION;
    self->base.base.dtypeInfo.dtype = DTYPE_U64;
    
    self->name = "<anonymous>";
    self->body = newAstCompound();
    self->returnTypeInfo.dtype    = DTYPE_UNKNOWN;
    self->returnTypeInfo.ptrCount = 0;

    self->scope = NULL;
}

astFunction_t* newAstFunction() {
    astFunction_t* self = (astFunction_t*) malloc(sizeof(astFunction_t));
    if (!self) error_malloc(__func__);

    AstFunction(self);
    return self;
}