#include "../include/ast/astCompound.h"

void CompoundPrint(astCompound_t* self, size_t indent) {
    INDENT(indent)
    printf("%s\n", astTypeToStr(self->base.type));
    
    for (size_t i = 0; i < self->children->size; ++i) {
        ast_t* child = (ast_t*) self->children->items[i];
        child->Print(child, indent + 1);
    }
}

void AstCompound(astCompound_t* self) {
    Ast(&self->base, AST_COMPOUND);    
    self->base.Print = (void (*)(ast_t*, size_t)) CompoundPrint;

    self->children = newList(sizeof(ast_t*));
}

astCompound_t* newAstCompound() {
    astCompound_t* self = (astCompound_t*) malloc(sizeof(astCompound_t));
    if (!self) error_malloc(__func__);

    AstCompound(self);
    return self;
}