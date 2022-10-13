#include "include/asm.h"

#include "include/ast/astAssignment.h"
#include "include/ast/astBinOp.h"
#include "include/ast/astCall.h"
#include "include/ast/astCast.h"
#include "include/ast/astFunction.h"
#include "include/ast/astConditionalStatement.h"
#include "include/ast/astUnaryStatement.h"
#include "include/ast/astUnaryOp.h"
#include "include/ast/astValue.h"
#include "include/ast/astVariable.h"

#include <string.h>

static char* dataBuf;
static char* textBuf;

static char* gen_root(const astCompound_t* ast);

static void gen_assignment(const astAssignment_t* ast, bool isRoot);
static void gen_function(const astFunction_t* ast, bool isRoot);
static void gen_compound(const astCompound_t* ast, bool isRoot);
static void gen_ast(const ast_t* ast, bool isRoot);

static void appendToData(const char* str) {
    dataBuf = realloc(dataBuf, strlen(dataBuf) + strlen(str) + 1);
    if (!dataBuf) error_realloc(__func__);
    strcat(dataBuf, str);
}

static void appendToText(const char* str) {
    textBuf = realloc(textBuf, strlen(textBuf) + strlen(str) + 1);
    if (!textBuf) error_realloc(__func__);
    strcat(textBuf, str);
}


static char* gen_root(const astCompound_t* ast) {
    char* template = \
    "bits 64\n\n" \
    "section .data\n" \
    "%s\n" \
    "section .text\n" \
    "default rel\n" \
    "%s";

    dataBuf = calloc(1, sizeof(char));
    textBuf = calloc(1, sizeof(char));

    gen_compound(ast, true);
    
    char* value = malloc(strlen(template) + strlen(dataBuf) + strlen(textBuf) + 1);
    sprintf(value, template, dataBuf, textBuf);

    return value;
}

char* gen_asm(const ast_t* root) {
    return gen_root((astCompound_t*)root);
}

static void gen_compound(const astCompound_t* ast, bool isRoot) {
    for (size_t i = 0; i < ast->children->size; ++i)
        gen_ast((ast_t*)ast->children->items[i], isRoot);
    
}

static void gen_ast(const ast_t* ast, bool isRoot) {
    switch (ast->type) {
        case AST_ASSIGNMENT:    gen_assignment((astAssignment_t*)ast, isRoot);  break;
        case AST_FUNCTION:      gen_function((astFunction_t*)ast, isRoot);      break;
        case AST_COMPOUND:      gen_compound((astCompound_t*)ast, isRoot);      break;
        default: error_gen_asm_type(ast->type);
    }
}

static char ibuf[20] = { 0 };
static void gen_function(const astFunction_t* ast, bool isRoot) {
    char* template =
        "\nglobal %s\n" 
        "%s:\n" 
        "push rbp\n" 
        "push rbx\n" 
        "mov rbp, rsp\n" 
        "sub rsp, %d\n\n";

    itoa(ast->scope->stackIndex, ibuf, 10);
    size_t stackSizeLength = strlen(ibuf);
    char* value = malloc(strlen(template) + strlen(ast->name) * 2 + stackSizeLength + 1);
    sprintf(value, template, ast->name, ast->name, abs(ast->scope->stackIndex));
    appendToText(value);
    free(value);
    // Going over the parameters
    for (size_t i = 0; i < ast->base.children->size && i < 4; ++i) {
        astVariable_t* child = (astVariable_t*)ast->base.children->items[i];
        appendToText("mov ");
        if (child->base.dtypeInfo.ptrCount) {
            appendToText("qword ");
        } else {
            switch (child->base.dtypeInfo.dtype) {
                case DTYPE_I8:
                case DTYPE_U8: appendToText("byte "); break;
                
                case DTYPE_I16:
                case DTYPE_U16: appendToText("word "); break;
                
                case DTYPE_I32:
                case DTYPE_U32: appendToText("dword "); break;
                
                case DTYPE_I64:
                case DTYPE_U64: appendToText("qword "); break;
                default: 
                    printf("[Assembly front-end]: TODO: %s is not supported.\n", dTypeToStr(child->base.dtypeInfo.dtype));
                    exit(1);
            }
        }
        itoa(child->stackIndex, ibuf, 10);
        template = "[rbp + %d], ";
        value = malloc(strlen(template) + strlen(ibuf) + 1);
        sprintf(value, template, child->stackIndex);
        appendToText(value);
        free(value);
        
        if (i < 2) {
            if (child->base.dtypeInfo.ptrCount) {
                appendToText("r");
            } else {
                switch (child->base.dtypeInfo.dtype) {
                case DTYPE_I32:
                case DTYPE_U32: appendToText("e"); break;
                
                case DTYPE_I64:
                case DTYPE_U64: appendToText("r"); break;
                default: break;
            }
            }
        }
        switch (i) {
            case 0: appendToText("cx\n"); break;
            case 1: appendToText("dx\n"); break;
            case 2: appendToText("r8\n");  break;
            case 3: appendToText("r9\n");  break;
        }
        if (i > 1) {
            if (!child->base.dtypeInfo.ptrCount) {
                switch (child->base.dtypeInfo.dtype) {
                    case DTYPE_I32:
                    case DTYPE_U32: appendToText("d"); break;
                    default: break;
                }
            }
        }
    }

    gen_compound(ast->body, false);

    template = 
        "\n_end_%s:\n" 
        "add rsp, %d\n" 
        "mov rsp, rbp\n" 
        "pop rbx\n" 
        "pop rbp\n" 
        "ret\n";

    value = malloc(strlen(template) + strlen(ast->name) + stackSizeLength + 1);
    sprintf(value, template, ast->name, abs(ast->scope->stackIndex));
    appendToText(value);
    free(value);

}

static void gen_assignment(const astAssignment_t* ast, bool isRoot) {
    if (isRoot) {
        if (ast->base.left->type != AST_VARIABLE) {
            printf("[Assembly front-end]: TODO: can't define an expression in global scope\n");
            exit(1);
        }
        // HELLO\n
        // 72, 69, 76, 76, 79, 10
        // "HELLO", 10
        astVariable_t* left = (astVariable_t*)ast->base.left;
        astValue_t* right = (astValue_t*)ast->base.right;

        appendToData(left->name);
        if (left->base.dtypeInfo.ptrCount && right->base.type != AST_STRING) {
            appendToData(" dq ");
        } else {
            switch (left->base.dtypeInfo.dtype) {
                case DTYPE_I8:
                case DTYPE_U8: appendToData(" db "); break;
                
                case DTYPE_I16:
                case DTYPE_U16: appendToData(" dw "); break;
                
                case DTYPE_I32:
                case DTYPE_U32: appendToData(" dd "); break;
                
                case DTYPE_I64:
                case DTYPE_U64: appendToData(" dq "); break;
                default: 
                    printf("[Assembly front-end]: TODO: %s is not supported.\n", dTypeToStr(left->base.dtypeInfo.dtype));
                    exit(1);
            }
        }
        // right side is constant
        if (right->base.type == AST_STRING) {
            size_t size = strlen(right->value);
            for (size_t i = 0; i < size; ++i) {
                appendToData((char[]){'\'', right->value[i], '\'', ',', ' ', 0});
            }
            appendToData("0\n");
            return;
        }
        appendToData(right->value);
        appendToData("\n");
        return;
    }

    ast_t* left = ast->base.left;
    ast_t* right = ast->base.right;

    switch (right->type) {
        case AST_INT:
            break;

        case AST_STRING:
            
            break;
        default: printf("[Assembly front-end]: TODO: don't know how to compile.\n"); exit(1);
    }
    

}