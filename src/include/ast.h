#pragma once

#include "types.h"
#include "list.h"
#include "scope.h"

// make clean if u want to update this
#define INDENT_SCALAR 4

typedef struct ast_t ast_t;

typedef enum astType_t {
    AST_NOOP,
    AST_ROOT,
    AST_COMPOUND,
    AST_CALL,    
    AST_FUNCTION,    
    AST_ASSIGNMENT,    
    AST_VARIABLE,
    AST_STATEMENT,
    AST_INT,
    AST_FLOAT,
    AST_STRING,
    AST_LIST,
} astType_t;

typedef enum statementType_t {
    STATEMENT_RETURN,
    STATEMENT_UNKNOWN,
} statementType_t;

typedef struct dtypeInfo_t {
    dType_t dtype;    
    int listCount; // int*, int** -> 1, 2
    dType_t typeOfList; // int* or char** stuff like this
} dtypeInfo_t;

struct ast_t {
    astType_t type;
    dtypeInfo_t dtypeInfo;

    // A virtual function
    void (*Print) (ast_t* self, size_t indent);
};

void AST(ast_t* self, astType_t type);
ast_t* newAST(astType_t type);

char* astTypeToStr(astType_t type);


// ******************************************* //
// * STATEMENT
typedef struct astStatement_t astStatement_t;
struct astStatement_t {
    ast_t base;
    statementType_t stype;
};

void ASTStatement(astStatement_t* self, statementType_t stype);
astStatement_t* newASTStatement(statementType_t stype);

char* statementTypeToStr(statementType_t type);
statementType_t strToStatementType(char* str);
// ******************************************* //


// ******************************************* //
// * COMPOUND
typedef struct astCompound_t astCompound_t;
struct astCompound_t {
    ast_t base;

    // list<ast_t*>
    list_t* children;
};
void ASTCompound(astCompound_t* self);
astCompound_t* newASTCompound();
// ******************************************* //


// ******************************************* //
// * CALL
typedef struct astCall_t astCall_t;
struct astCall_t {
    ast_t base;

    char* name;
    astCompound_t* value;  // parameters
};
void ASTCall(astCall_t* self, char* name, astCompound_t* arguments);
astCall_t* newASTCall(char* name, astCompound_t* arguments);
// ******************************************* //


// ******************************************* //
// * FUNCTION
typedef struct astFunction_t astFunction_t;
struct astFunction_t {
    astCompound_t base;

    char* name;
    dtypeInfo_t returnTypeInfo;
    astCompound_t* value;
    scope_t* scope;
};
void ASTFunction(astFunction_t* self);
astFunction_t* newASTFunction();
// ******************************************* //


// ******************************************* //
// * ASSIGNMENT
typedef struct astAssignment_t astAssignment_t;
struct astAssignment_t {
    ast_t base;

    char* name;
    ast_t* value;
};
void ASTAssignment(astAssignment_t* self, char* name, ast_t* value);
astAssignment_t* newASTAssignment(char* name, ast_t* value);
// ******************************************* //


// ******************************************* //
// * VARIABLE
typedef struct astVariable_t astVariable_t;
struct astVariable_t {
    ast_t base;
    int stackIndex;

    char* name;
};
void ASTVariable(astVariable_t* self, char* name);
astVariable_t* newASTVariable(char* name);
// ******************************************* //


// ******************************************* //
// * INT
typedef struct astInt_t astInt_t;
struct astInt_t {
    ast_t base;

    int value;
};
void ASTInt(astInt_t* self, int value);
astInt_t* newASTInt(int value);
// ******************************************* //


// ******************************************* //
// * RETURN STATEMENT
typedef struct astStatementReturn_t astStatementReturn_t;
struct astStatementReturn_t {
    astStatement_t base;

    ast_t* value;
};

void ASTStatementReturn(astStatementReturn_t* self, ast_t* value);
astStatementReturn_t* newASTStatementReturn(ast_t* value);
// ******************************************* //
