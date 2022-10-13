#include "../include/ast/astCall.h"

void CallPrint(astCall_t* self, size_t indent);

void AstCall(astCall_t* self, ast_t* value, astCompound_t* arguments) {
    Ast(&self->base, AST_CALL);
    self->base.Print = (void (*)(ast_t*, size_t)) CallPrint;

    self->arguments = arguments;
    self->value = value;
}

astCall_t* newAstCall(ast_t* value, astCompound_t* arguments) {
    astCall_t* self = (astCall_t*) malloc(sizeof(astCall_t));
    if (!self) error_malloc(__func__);

    AstCall(self, value, arguments);
    return self;
}

void CallPrint(astCall_t* self, size_t indent) {
    INDENT(indent)
    printf("%s:\n", astTypeToStr(self->base.type));

    INDENT(indent + 1)
    printf("Calling:\n");
    self->value->Print(self->value, indent + 2);
    
    for (size_t i = 0; i < self->arguments->children->size; ++i) {
        INDENT(indent + 1)
        printf("Argument #%lld:\n", i);
        ast_t* child = (ast_t*) self->arguments->children->items[i];
        child->Print(child, indent + 2);
    }
}