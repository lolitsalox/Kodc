#include "../include/ast/astAssignment.h"

void AssignmentPrint(astAssignment_t* self, size_t indent);

void AstAssignment(astAssignment_t* self, bool typeDefined, ast_t* left, ast_t* right) {
    AstBin(&self->base, AST_ASSIGNMENT, left, right);
    self->base.base.Print = (void (*)(ast_t*, size_t)) AssignmentPrint;

    self->typeDefined = typeDefined;
}

astAssignment_t* newAstAssignment(bool typeDefined, ast_t* left, ast_t* right) {
    astAssignment_t* self = (astAssignment_t*) malloc(sizeof(astAssignment_t));
    if (!self) error_malloc(__func__);

    AstAssignment(self, typeDefined, left, right);
    return self;
}

void AssignmentPrint(astAssignment_t* self, size_t indent) {
    INDENT(indent)
    printf("%s - typeDefined = %s\n", astTypeToStr(self->base.base.type), self->typeDefined ? "true" : "false");

    INDENT(indent + 1)
    printf("left:\n");
    self->base.left->Print(self->base.left, indent + 2);
    
    INDENT(indent + 1)
    printf("right:\n");
    self->base.right->Print(self->base.right, indent + 2);
}