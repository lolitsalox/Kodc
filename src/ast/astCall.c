#include "../include/ast/astCall.h"

void AstCall(astCall_t* self, ast_t* value, astCompound_t* arguments) {
    Ast(&self->base, AST_CALL);
    // self->base.Print = (void (*)(ast_t*, size_t)) CallPrint;

    self->arguments = arguments;
    self->value = value;
}

astCall_t* newAstCall(ast_t* value, astCompound_t* arguments) {
    astCall_t* self = (astCall_t*) malloc(sizeof(astCall_t));
    if (!self) error_malloc(__func__);

    AstCall(self, value, arguments);
    return self;
}