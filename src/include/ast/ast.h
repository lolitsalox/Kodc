#pragma once

#include "../types.h"
#include <stdio.h>
#include <stdlib.h>

// make clean if u want to update this
#define INDENT_SCALAR 4
#define INDENT(x) for (size_t i = 0; i < (x) * INDENT_SCALAR; ++i) \
                    printf(" ");

typedef enum astType_t {
    AST_NOOP,
    AST_ROOT,
    AST_COMPOUND,
    AST_CALL,    
    AST_UNARY_OP,    
    AST_BIN_OP,    
    AST_FUNCTION,    
    AST_ASSIGNMENT,    
    AST_VARIABLE,
    AST_CAST,
    AST_STATEMENT,
    AST_CONDITIONAL_STATEMENT,
    AST_UNARY_STATEMENT,
    AST_ADDRESS,
    AST_INT,
    AST_FLOAT,
    AST_STRING,
    AST_BOOL,
} astType_t;

// Converts astType_t to a string literal
char* astTypeToStr(astType_t type);

#include "../error.h"

typedef struct ast_t ast_t;

// A struct that defines the header of the AST node
struct ast_t {

    // The type of the ast node
    astType_t type;

    // Additional info about the datatype if needed
    dtypeInfo_t dtypeInfo;

    // Return dtype if needed
    dtypeInfo_t dtypeReturnInfo;

    // A virtual function
    void (*Print) (ast_t* self, size_t indent);

};

// Constructs the ast node
void Ast(ast_t* self, astType_t type);

// Allocates heap memory and constructs the ast node
ast_t* newAst(astType_t type);

// TODO: implement print for all the nodes