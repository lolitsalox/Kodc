#include "include/ast.h"
#include "include/error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ASTPrint(ast_t* self, size_t indent) {
    for (size_t i = 0; i < indent * INDENT_SCALAR; ++i)
        printf(" ");
    printf("%s\n", astTypeToStr(self->type));
}

void AST(ast_t* self, astType_t type) {
    self->type = type;
    self->dtypeInfo.dtype = DTYPE_UNKNOWN;
    self->dtypeInfo.typeOfList = DTYPE_UNKNOWN;
    self->dtypeInfo.listCount = 0;

    self->Print = ASTPrint;
}

ast_t* newAST(astType_t type) {
    ast_t* self = (ast_t*) malloc(sizeof(ast_t));
    if (!self)
        error_malloc(__func__);

    AST(self, type);
    return self;
}

char* astTypeToStr(astType_t type) {
    switch (type) {
        case AST_NOOP:          return "AST_NOOP";
        case AST_ROOT:          return "AST_ROOT";
        case AST_COMPOUND:      return "AST_COMPOUND";
        case AST_CALL:          return "AST_CALL";
        case AST_FUNCTION:      return "AST_FUNCTION";
        case AST_ASSIGNMENT:    return "AST_ASSIGNMENT";
        case AST_VARIABLE:      return "AST_VARIABLE";
        case AST_STATEMENT:     return "AST_STATEMENT";
        case AST_INT:           return "AST_INT";
        case AST_FLOAT:         return "AST_FLOAT";
        case AST_STRING:        return "AST_STRING";
        case AST_LIST:          return "AST_LIST";
        default:                return "AST_UNKNOWN";
    }
}


// ****************************************** //
// * STATEMENT
void ASTStatement(astStatement_t* self, statementType_t stype) {
    AST(&self->base, AST_STATEMENT);
    self->stype = stype;
}

astStatement_t* newASTStatement(statementType_t stype) {
    astStatement_t* self = (astStatement_t*) malloc(sizeof(astStatement_t));
    if (!self)
        error_malloc(__func__);

    ASTStatement(self, stype);
    return self;
}

char* statementTypeToStr(statementType_t type) {
    switch (type) {
        case STATEMENT_RETURN:          return "STATEMENT_RETURN";
        default:                        return "STATEMENT_UNKNOWN";
    }
}

statementType_t strToStatementType(char* str) {
    if (strcmp(str, "return") == 0) return STATEMENT_RETURN;
    return STATEMENT_UNKNOWN;
}
// ****************************************** //


// ****************************************** //
// * COMPOUND
void CompoundPrint(astCompound_t* self, size_t indent) {
    for (size_t i = 0; i < indent * INDENT_SCALAR; ++i)
        printf(" ");
    printf("%s\n", astTypeToStr(self->base.type));
    
    for (size_t i = 0; i < self->children->size; ++i) {
        ast_t* child = (ast_t*) self->children->items[i];
        child->Print(child, indent + 1);
    }
}

void ASTCompound(astCompound_t* self) {
    AST(&self->base, AST_COMPOUND);    
    self->base.Print = (void (*)(ast_t*, size_t)) CompoundPrint;

    self->children = newList(sizeof(ast_t*));
}

astCompound_t* newASTCompound() {
    astCompound_t* self = (astCompound_t*) malloc(sizeof(astCompound_t));
    if (!self)
        error_malloc(__func__);

    ASTCompound(self);
    return self;
}
// ****************************************** //


// ****************************************** //
// * Call
void CallPrint(astCall_t* self, size_t indent) {
    for (size_t i = 0; i < indent * INDENT_SCALAR; ++i)
        printf(" ");
    printf("%s - name: %s\n", astTypeToStr(self->base.type), self->name);
    
    for (size_t i = 0; i < self->value->children->size; ++i) {
        for (size_t i = 0; i < (indent + 1) * INDENT_SCALAR; ++i)
            printf(" ");
        printf("Argument #%lld:\n", i);
        ast_t* child = (ast_t*) self->value->children->items[i];
        child->Print(child, indent + 2);
    }
}

void ASTCall(astCall_t* self, char* name, astCompound_t* arguments) {
    AST(&self->base, AST_CALL);
    self->base.Print = (void (*)(ast_t*, size_t)) CallPrint;

    self->value = arguments;
    self->name = name;
}

astCall_t* newASTCall(char* name, astCompound_t* arguments) {
    astCall_t* self = (astCall_t*) malloc(sizeof(astCall_t));
    if (!self)
        error_malloc(__func__);

    ASTCall(self, name, arguments);
    return self;
}
// ****************************************** //


// ****************************************** //
// * Function
void FunctionPrint(astFunction_t* self, size_t indent) {
    for (size_t i = 0; i < indent * INDENT_SCALAR; ++i)
        printf(" ");
    printf("%s\n", astTypeToStr(self->base.base.type));
    for (size_t i = 0; i < (indent + 1) * INDENT_SCALAR; ++i)
        printf(" ");
    printf("name: %s\n", self->name);
    for (size_t i = 0; i < (indent + 1) * INDENT_SCALAR; ++i)
        printf(" ");
    printf("return type: %s", dTypeToStr(self->returnTypeInfo.dtype));

    if (self->returnTypeInfo.listCount) {
        for (int i = 0; i < self->returnTypeInfo.listCount - 1; ++i)
            printf("<%s", dTypeToStr(DTYPE_LIST));
        printf("<%s>", dTypeToStr(self->returnTypeInfo.typeOfList));
        for (int i = 0; i < self->returnTypeInfo.listCount - 1; ++i)
            printf(">");
    }
    printf("\n");
    
    for (size_t i = 0; i < self->base.children->size; ++i) {
        for (size_t i = 0; i < (indent + 1) * INDENT_SCALAR; ++i)
            printf(" ");
        printf("Parameter #%lld:\n", i);
        ast_t* child = (ast_t*) self->base.children->items[i];
        child->Print(child, indent + 2);
    }
    CompoundPrint(self->value, indent + 1);
}

void ASTFunction(astFunction_t* self) {
    ASTCompound(&self->base);
    self->value = newASTCompound();
    self->base.base.Print = (void (*)(ast_t*, size_t)) FunctionPrint;
    self->base.base.type = AST_FUNCTION;
    self->base.base.dtypeInfo.dtype = DTYPE_U64;
    
    self->returnTypeInfo.dtype      = DTYPE_UNKNOWN;
    self->returnTypeInfo.typeOfList = DTYPE_UNKNOWN;
    self->returnTypeInfo.listCount  = 0;

    self->name = "<anonymous>";

    self->scope = NULL;
}

astFunction_t* newASTFunction() {
    astFunction_t* self = (astFunction_t*) malloc(sizeof(astFunction_t));
    if (!self)
        error_malloc(__func__);

    ASTFunction(self);
    return self;
}
// ****************************************** //


// ****************************************** //
// * Assignment
void AssignmentPrint(astAssignment_t* self, size_t indent) {
    for (size_t i = 0; i < indent * INDENT_SCALAR; ++i)
        printf(" ");
    printf("%s\n", astTypeToStr(self->base.type));
    for (size_t i = 0; i < (indent + 1) * INDENT_SCALAR; ++i)
        printf(" ");
    printf("name: %s\n", self->name);
    for (size_t i = 0; i < (indent + 1) * INDENT_SCALAR; ++i)
        printf(" ");
    printf("type: %s", dTypeToStr(self->base.dtypeInfo.dtype));

    if (self->base.dtypeInfo.listCount) {
        for (int i = 0; i < self->base.dtypeInfo.listCount - 1; ++i)
            printf("<%s", dTypeToStr(DTYPE_LIST));
        printf("<%s>", dTypeToStr(self->base.dtypeInfo.typeOfList));
        for (int i = 0; i < self->base.dtypeInfo.listCount - 1; ++i)
            printf(">");
    }
    printf("\n");

    self->value->Print(self->value, indent + 1);
}

void ASTAssignment(astAssignment_t* self, char* name, ast_t* value) {
    AST(&self->base, AST_ASSIGNMENT);
    self->base.Print = (void (*)(ast_t*, size_t)) AssignmentPrint;

    self->name = name;
    self->value = value;
}

astAssignment_t* newASTAssignment(char* name, ast_t* value) {
    astAssignment_t* self = (astAssignment_t*) malloc(sizeof(astAssignment_t));
    if (!self)
        error_malloc(__func__);

    ASTAssignment(self, name, value);
    return self;
}
// ****************************************** //


// ****************************************** //
// * Variable
void VariablePrint(astVariable_t* self, size_t indent) {
    for (size_t i = 0; i < indent * INDENT_SCALAR; ++i)
        printf(" ");
    printf("%s\n", astTypeToStr(self->base.type));
    for (size_t i = 0; i < (indent + 1) * INDENT_SCALAR; ++i)
        printf(" ");
    printf("name: %s\n", self->name);

    for (size_t i = 0; i < (indent + 1) * INDENT_SCALAR; ++i)
        printf(" ");
    printf("type: %s", dTypeToStr(self->base.dtypeInfo.dtype));

    if (self->base.dtypeInfo.listCount) {
        for (int i = 0; i < self->base.dtypeInfo.listCount - 1; ++i)
            printf("<%s", dTypeToStr(DTYPE_LIST));
        printf("<%s>", dTypeToStr(self->base.dtypeInfo.dtype));
        for (int i = 0; i < self->base.dtypeInfo.listCount - 1; ++i)
            printf(">");
    }
    printf("\n");
}

void ASTVariable(astVariable_t* self, char* name) {
    AST(&self->base, AST_VARIABLE);
    self->base.Print = (void (*)(ast_t*, size_t)) VariablePrint;

    self->stackIndex = 0;

    self->name = name;
}

astVariable_t* newASTVariable(char* name) {
    astVariable_t* self = (astVariable_t*) malloc(sizeof(astVariable_t));
    if (!self)
        error_malloc(__func__);

    ASTVariable(self, name);
    return self;
}
// ****************************************** //


// ******************************************* //
// * Int
void IntPrint(astInt_t* self, size_t indent) {
    for (size_t i = 0; i < indent * INDENT_SCALAR; ++i)
        printf(" ");
    printf("%s - value: %d\n", astTypeToStr(self->base.type), self->value);
}

void ASTInt(astInt_t* self, int value) {
    AST(&self->base, AST_INT);
    self->base.Print = (void (*)(ast_t*, size_t)) IntPrint;
    self->base.dtypeInfo.dtype = DTYPE_I32; // setting a number as i32 by default

    self->value = value;
}

astInt_t* newASTInt(int value) {
    astInt_t* self = (astInt_t*) malloc(sizeof(astInt_t));
    if (!self)
        error_malloc(__func__);

    ASTInt(self, value);
    return self;
}
// ******************************************* //


// ******************************************* //
// * RETURN STATEMENT
void ASTStatementReturn(astStatementReturn_t* self, ast_t* value) {
    ASTStatement(&self->base, STATEMENT_RETURN);
    self->value = value;
}

astStatementReturn_t* newASTStatementReturn(ast_t* value) {
    astStatementReturn_t* self = (astStatementReturn_t*) malloc(sizeof(astStatementReturn_t));
    if (!self)
        error_malloc(__func__);

    ASTStatementReturn(self, value);
    return self;
}
// ******************************************* //
