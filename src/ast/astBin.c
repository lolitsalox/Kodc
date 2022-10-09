#include "../include/ast/astBin.h"

void AssignmentPrint(astBin_t* self, size_t indent);

void AstBin(astBin_t* self, astType_t type, ast_t* left, ast_t* right) {
    Ast(&self->base, type);

    self->left = left;
    self->right = right;

    switch (type) {
        case AST_ASSIGNMENT:
            self->base.Print = (void (*)(ast_t*, size_t)) AssignmentPrint;
            break;
        default: break;
    }
}

astBin_t* newAstBin(astType_t type, ast_t* left, ast_t* right) {
    astBin_t* self = (astBin_t*) malloc(sizeof(astBin_t));
    if (!self) error_malloc(__func__);

    AstBin(self, type, left, right);
    return self;
}

void AssignmentPrint(astBin_t* self, size_t indent) {
    INDENT(indent)
    printf("%s\n", astTypeToStr(self->base.type));
    
    INDENT(indent + 1)
    printf("left:\n");
    self->left->Print(self->left, indent + 2);
    
    INDENT(indent + 1)
    printf("right:\n");
    self->right->Print(self->right, indent + 2);
}