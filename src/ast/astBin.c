#include "../include/ast/astBin.h"

void AstBin(astBin_t* self, astType_t type, ast_t* left, ast_t* right) {
    Ast(&self->base, type);

    self->left = left;
    self->right = right;
}

astBin_t* newAstBin(astType_t type, ast_t* left, ast_t* right) {
    astBin_t* self = (astBin_t*) malloc(sizeof(astBin_t));
    if (!self) error_malloc(__func__);

    AstBin(self, type, left, right);
    return self;
}
