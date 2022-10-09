#include "../include/ast/astBinOp.h"

void AstBinOp(astBinOp_t* self, tokenType_t op, ast_t* left, ast_t* right) {
    AstBin(&self->base, AST_BIN_OP, left, right);

    self->op = op;
}

astBinOp_t* newAstBinOp(tokenType_t op, ast_t* left, ast_t* right) {
    astBinOp_t* self = (astBinOp_t*) malloc(sizeof(astBinOp_t));
    if (!self) error_malloc(__func__);

    AstBinOp(self, op, left, right);
    return self;
}
