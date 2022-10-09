#include "../include/ast/astUnaryOp.h"

void AstUnaryOp(astUnaryOp_t* self, tokenType_t op, ast_t* value) {
    Ast(&self->base, AST_UNARY_OP);

    self->op = op;
    self->value = value;
}

astUnaryOp_t* newAstUnaryOp(tokenType_t op, ast_t* value) {
    astUnaryOp_t* self = (astUnaryOp_t*) malloc(sizeof(astUnaryOp_t));
    if (!self) error_malloc(__func__);

    AstUnaryOp(self, op, value);
    return self;
}
