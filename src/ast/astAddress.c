#include "../include/ast/astAddress.h"

void AddressPrint(astAddress_t* self, size_t indent);

void AstAddress(astAddress_t* self, bool ref, ast_t* value) {
    Ast(&self->base, AST_ADDRESS);
    self->base.Print = (void (*)(ast_t*, size_t)) AddressPrint;
    
    self->ref = ref;
    self->value = value;
}

astAddress_t* newAstAddress(bool ref, ast_t* value) {
    astAddress_t* self = (astAddress_t*) malloc(sizeof(astAddress_t));
    if (!self) error_malloc(__func__);

    AstAddress(self, ref, value);
    return self;
}

void AddressPrint(astAddress_t* self, size_t indent) {
    INDENT(indent)
    printf("%s - %c\n", astTypeToStr(self->base.type), self->ref ? '#' : '@');
    self->value->Print(self->value, indent + 1);
}