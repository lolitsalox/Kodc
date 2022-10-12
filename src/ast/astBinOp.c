#include "../include/ast/astBinOp.h"

void AstBinOpPrint(astBinOp_t* self, size_t indent);

void AstBinOp(astBinOp_t* self, tokenType_t op, ast_t* left, ast_t* right) {
    AstBin(&self->base, AST_BIN_OP, left, right);
    self->base.base.Print = (void (*)(ast_t*, size_t)) AstBinOpPrint;

    self->op = op;
}

astBinOp_t* newAstBinOp(tokenType_t op, ast_t* left, ast_t* right) {
    astBinOp_t* self = (astBinOp_t*) malloc(sizeof(astBinOp_t));
    if (!self) error_malloc(__func__);

    AstBinOp(self, op, left, right);
    return self;
}

void AstBinOpPrint(astBinOp_t* self, size_t indent) {
    INDENT(indent)
    printf("%s - %s\n", astTypeToStr(self->base.base.type), tokenTypeToStr(self->op));

    INDENT(indent + 1)
    printf("left:\n");
    self->base.left->Print(self->base.left, indent + 2);
    
    INDENT(indent + 1)
    printf("right:\n");
    self->base.right->Print(self->base.right, indent + 2);
}