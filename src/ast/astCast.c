#include "../include/ast/astCast.h"

void AstCast(astCast_t* self, dtypeInfo_t dtypeInfo, ast_t* expr) {
    Ast(&self->base, AST_CAST);

    self->dtypeInfo = dtypeInfo;
    self->expr = expr;
}

astCast_t* newAstCast(dtypeInfo_t dtypeInfo, ast_t* expr) {
    astCast_t* self = (astCast_t*) malloc(sizeof(astCast_t));
    if (!self) error_malloc(__func__);

    AstCast(self, dtypeInfo, expr);
    return self;
}