#include "../include/ast/astUnaryStatement.h"

void UnaryStatementPrint(astUnaryStatement_t* self, size_t indent);

void AstUnaryStatement(astUnaryStatement_t* self, statementType_t stype, ast_t* value) {
    AstStatement(&self->base, stype);
    self->base.base.type = AST_UNARY_STATEMENT;
    self->base.base.Print = (void (*)(ast_t*, size_t)) UnaryStatementPrint;

    self->value = value;
}

astUnaryStatement_t* newAstUnaryStatement(statementType_t stype, ast_t* value) {
    astUnaryStatement_t* self = (astUnaryStatement_t*) malloc(sizeof(astUnaryStatement_t));
    if (!self) error_malloc(__func__);

    AstUnaryStatement(self, stype, value);
    return self;
}

void UnaryStatementPrint(astUnaryStatement_t* self, size_t indent) {
    INDENT(indent)
    printf("%s: - %s\n", astTypeToStr(self->base.base.type), stype_to_str(self->base.stype));

    self->value->Print(self->value, indent + 1);
}