#include "../include/ast/astUnaryOp.h"

void AstUnaryOpPrint(astUnaryOp_t* self, size_t indent);

void AstUnaryOp(astUnaryOp_t* self, tokenType_t op, ast_t* value) {
    Ast(&self->base, AST_UNARY_OP);
    self->base.Print = (void (*)(ast_t*, size_t)) AstUnaryOpPrint;

    self->op = op;
    self->value = value;
}

astUnaryOp_t* newAstUnaryOp(tokenType_t op, ast_t* value) {
    astUnaryOp_t* self = (astUnaryOp_t*) malloc(sizeof(astUnaryOp_t));
    if (!self) error_malloc(__func__);

    AstUnaryOp(self, op, value);
    return self;
}

void AstUnaryOpPrint(astUnaryOp_t* self, size_t indent) {
    INDENT(indent)
    printf("%s - %s\n", astTypeToStr(self->base.type), tokenTypeToStr(self->op));

    INDENT(indent + 1)
    printf("value:\n");
    self->value->Print(self->value, indent + 2);
}