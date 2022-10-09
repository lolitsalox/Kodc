#include "../include/ast/astAddress.h"

void AstAddress(astAddress_t* self, bool ref, ast_t* value) {
    Ast(&self->base, AST_ADDRESS);
    // self->base.Print = (void (*)(ast_t*, size_t)) AddressPrint;
    
    self->ref = ref;
    self->value = value;
}

astAddress_t* newAstAddress(bool ref, ast_t* value) {
    astAddress_t* self = (astAddress_t*) malloc(sizeof(astAddress_t));
    if (!self) error_malloc(__func__);

    AstAddress(self, ref, value);
    return self;
}
