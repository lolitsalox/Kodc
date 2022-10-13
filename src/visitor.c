#include "include/visitor.h"
#include "include/error.h"
// #include "include/types.h"

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ast_t* lookupVar(scope_t* scope, char* name) {
    if (scope->mom) {
        ast_t* var = lookupVar(scope->mom, name);
        if (var) return var;
    }

    for (size_t i = 0; i < scope->variables->size; ++i) {
        ast_t* child = (ast_t*) scope->variables->items[i];
        if (child->type == AST_VARIABLE) {
            if (strcmp(((astVariable_t*)child)->name, name) == 0) {
                return child;
            }
        } else {
            if (strcmp(((astFunction_t*)child)->name, name) == 0) {
                return child;
            }
        }
    }
    return NULL;
}

ast_t* getFirstInside(astCompound_t* node) {
    if (node->children->size > 0) {
        ast_t* child = node->children->items[0];
        if (child->type == AST_COMPOUND)
            return getFirstInside((astCompound_t*)child);
        return child;
    }
    return NULL;
}

ast_t* Visit(visitor_t* self, ast_t* node, scope_t* scope, bool noerror);

ast_t* VisitCompound(visitor_t* self, astCompound_t* node, scope_t* scope);
ast_t* VisitAssignment(visitor_t* self, astAssignment_t* node, scope_t* scope);
ast_t* VisitVariable(visitor_t* self, astVariable_t* node, scope_t* scope, bool noerror);
ast_t* VisitFunction(visitor_t* self, astFunction_t* node, scope_t* scope);
ast_t* VisitCall(visitor_t* self, astCall_t* node, scope_t* scope);
ast_t* VisitBinOp(visitor_t* self, astBinOp_t* node, scope_t* scope);
ast_t* VisitUnaryOp(visitor_t* self, astUnaryOp_t* node, scope_t* scope);
ast_t* VisitUnaryStatement(visitor_t* self, astUnaryStatement_t* node, scope_t* scope);
ast_t* VisitConditionalStatement(visitor_t* self, astConditionalStatement_t* node, scope_t* scope);


ast_t* Visit(visitor_t* self, ast_t* node, scope_t* scope, bool noerror) {
    switch (node->type) {
        case AST_INT:
        case AST_FLOAT:
        case AST_STRING:        return node;
        case AST_ROOT:
        case AST_COMPOUND:      return VisitCompound    (self, (astCompound_t*) node, scope);
        case AST_ASSIGNMENT:    return VisitAssignment  (self, (astAssignment_t*) node, scope);
        case AST_VARIABLE:      return VisitVariable    (self, (astVariable_t*) node, scope, noerror);
        case AST_FUNCTION:      return VisitFunction    (self, (astFunction_t*) node, scope);
        case AST_CALL:          return VisitCall        (self, (astCall_t*) node, scope);
        case AST_BIN_OP:        return VisitBinOp       (self, (astBinOp_t*) node, scope);
        case AST_UNARY_OP:      return VisitUnaryOp     (self, (astUnaryOp_t*) node, scope);
        case AST_UNARY_STATEMENT:           return VisitUnaryStatement          (self, (astUnaryStatement_t*) node, scope);
        case AST_CONDITIONAL_STATEMENT:     return VisitConditionalStatement    (self, (astConditionalStatement_t*) node, scope);
        default: error_visitor_type(node->type);
    }

    return NULL;
}

void Visitor(visitor_t* self) {
    self->Visit = Visit;
    self->globalScope = newScope(NULL);
    self->currentFunction = NULL;    
}

visitor_t* newVisitor() {
    visitor_t* self = (visitor_t*) malloc(sizeof(visitor_t));
    if (!self) error_malloc(__func__);

    Visitor(self);
    return self;
}

ast_t* VisitCompound(visitor_t* self, astCompound_t* node, scope_t* scope) {
    for (size_t i = 0; i < ((astCompound_t*)node)->children->size; ++i)
        node->children->items[i] = Visit(self, node->children->items[i], scope, false);
    
    return (ast_t*) node;
}

ast_t* VisitAssignment(visitor_t* self, astAssignment_t* node, scope_t* scope) {
    node->base.left = Visit(self, node->base.left, scope, true);
    node->base.right = Visit(self, node->base.right, scope, false);

    if (node->base.left->type == AST_VARIABLE) {
        astVariable_t* var = (astVariable_t*) lookupVar(scope, ((astVariable_t*)node->base.left)->name);

        if (var) {
            node->base.left = (ast_t*) var;

            // x: i32 = 3
            // x: #i8 = "3"
            // ^ or in global scope
            if (node->typeDefined || !scope->mom) {
                printf("[Visitor]: TODO: Defining a variable twice\n");
                exit(1);
            }

            // x = 5
            // x = "5"
            if (node->base.right->dtypeInfo.dtype != var->base.dtypeInfo.dtype ||
                node->base.right->dtypeInfo.ptrCount != var->base.dtypeInfo.ptrCount
            ) {
                printf("[Visitor]: TODO: assigning a different dtype (1)\n");
                exit(1);
            }

            return (ast_t*) node;
        }
    } else if (!scope->mom) {
        printf("[Visitor]: TODO: can't define an expression inside the global scope\n");
        exit(1);
    }

    // TODO: add a check for const values in the global scope

    if (node->typeDefined && (
        node->base.left->dtypeInfo.dtype != node->base.right->dtypeInfo.dtype ||
        node->base.left->dtypeInfo.ptrCount != node->base.right->dtypeInfo.ptrCount
    )) {
        printf("[Visitor]: TODO: assigning a different dtype\n");
        exit(1);
    }

    node->base.left->dtypeInfo = node->base.right->dtypeInfo;
    node->base.left->dtypeReturnInfo = node->base.right->dtypeReturnInfo;
    
    scope->variables->PushBack(scope->variables, node->base.left);
    
    if (scope->mom) {
        scope->stackIndex -= dtypeInfoSize(((astVariable_t*)node->base.left)->base.dtypeInfo);
        ((astVariable_t*)node->base.left)->stackIndex = scope->stackIndex;
    }

    return (ast_t*) node;
}

ast_t* VisitVariable(visitor_t* self, astVariable_t* node, scope_t* scope, bool noerror) {
    ast_t* var = (ast_t*) lookupVar(scope, node->name);


    if (var) return var;
    if (noerror) return (ast_t*) node;

    printf("[Visitor]: '%s' is undefined\n", node->name);
    exit(1);
    return NULL;
}

ast_t* VisitFunction(visitor_t* self, astFunction_t* node, scope_t* scope) {
    node->scope = newScope(scope);
    self->currentFunction = node;

    int tSp = 16; // rip, rbp
    for (size_t i = 0; i < node->base.children->size; ++i) {
        astVariable_t* child = (astVariable_t*) node->base.children->items[i];

        if (child->base.type != AST_VARIABLE || !child->typeDefined) {
            printf("[Visitor]: TODO: invalid function definition\n");
            exit(1);
        }

        node->scope->variables->PushBack(node->scope->variables, child);
        
        child->stackIndex = tSp;
        tSp += dtypeInfoSize(child->base.dtypeInfo);
        
    }
    node->scope->paramSize = tSp - 16;

    if (strcmp(node->name, "<anonymous>") != 0) {
        scope->variables->PushBack(scope->variables, node);
    }

    self->currentFunction = node;

    for (size_t i = 0; i < node->body->children->size; ++i) {
        node->body->children->items[i] = Visit(self, (ast_t*) node->body->children->items[i], node->scope, false);
    }

    // * To check the allocations, uncomment
    printf("function %s:\n", node->name);
    printf("Need to allocate %d bytes for the params\nAnd %d bytes for the function stack frame\n", node->scope->paramSize, -node->scope->stackIndex);
    for (int i = 0; i < node->scope->variables->size; ++i) {
        if (((ast_t*)node->scope->variables->items[i])->type != AST_VARIABLE) continue;
        int si = ((astVariable_t*)node->scope->variables->items[i])->stackIndex;
        printf("#%d: '%s' [rbp %c %d]\n", i,  ((astVariable_t*)node->scope->variables->items[i])->name, si > 0 ? '+' : '-', abs(si));
    }
    printf("\n");


    if (strcmp(node->name, "<anonymous>") == 0) {
        return (ast_t*) node;
    }

    return (ast_t*) node;
}


ast_t* VisitCall(visitor_t* self, astCall_t* node, scope_t* scope) {
    if (node->value->type != AST_VARIABLE) {
        printf("[Visitor]: TODO: can't call something else than a variable\n");
        exit(1);
    }
    ast_t* func = lookupVar(scope, ((astVariable_t*)node->value)->name);

    if (!func) {
        printf("[Visitor]: TODO: function '%s' not found\n", ((astVariable_t*)node->value)->name);
        exit(1);
    }

    // if (!func) {
    //     // Search for a builtin
    //     if (strcmp(node->name, "return") == 0) {
    //         for (size_t i = 0; i < node->value->children->size; ++i) {
    //             Visit(self, node->value->children->items[i], scope);
    //         }
    //         if (!self->currentFunction) {
    //             printf("[Visitor]: Returning at global scope\n");
    //             exit(1);
    //         }
            

    //         if (node->value->children->size == 1) {
    //             if (self->currentFunction->returnTypeInfo.dtype == DTYPE_UNKNOWN) {
    //                 self->currentFunction->returnTypeInfo = ((ast_t*)node->value->children->items[0])->dtypeInfo;
    //                 return (ast_t*) node;
    //             }
    //             if (self->currentFunction->returnTypeInfo.dtype != ((ast_t*)node->value->children->items[0])->dtypeInfo.dtype ||
    //                 self->currentFunction->returnTypeInfo.ptrCount != ((ast_t*)node->value->children->items[0])->dtypeInfo.ptrCount
    //             ) {
    //                 error_visitor_return_type_mismatch(
    //                     self->currentFunction->name, 
    //                     dTypeToStr(self->currentFunction->returnTypeInfo.dtype),
    //                     dTypeToStr(((ast_t*)node->value->children->items[0])->dtypeInfo.dtype)
    //                 );
    //             }
    //             return (ast_t*) node;
    //         }


            
    //         return (ast_t*) node;
    //     }
        
    //     error_visitor_undefined(node->name);
    // }

    for (size_t i = 0; i < node->arguments->children->size; ++i) {
        node->arguments->children->items[i] = Visit(self, (ast_t*) node->arguments->children->items[i], scope, false);
    }

    for (size_t i = 0; i < node->arguments->children->size; ++i) {
        node->arguments->children->items[i] = Visit(self, (ast_t*) node->arguments->children->items[i], scope, false);
    }

    node->base.dtypeInfo = func->dtypeReturnInfo;
    // printf("return type = %s, %lld\n", dTypeToStr(func->returnTypeInfo.dtype), func->returnTypeInfo.ptrCount);

    return (ast_t*) node;
}

ast_t* VisitBinOp(visitor_t* self, astBinOp_t* node, scope_t* scope) {
    node->base.left = Visit(self, node->base.left, scope, false);
    node->base.right = Visit(self, node->base.right, scope, false);

    if (node->base.left->dtypeInfo.dtype != node->base.right->dtypeInfo.dtype ||
        node->base.left->dtypeInfo.ptrCount != node->base.right->dtypeInfo.ptrCount
    ) {
        printf("[Vistor]: TODO: operands are not the same\n");
        exit(1);
    }

    node->base.base.dtypeInfo = node->base.left->dtypeInfo;
    return (ast_t*) node;
}

ast_t* VisitUnaryOp(visitor_t* self, astUnaryOp_t* node, scope_t* scope) {
    
    switch (node->op) {
        case TOKEN_HASH:
            node->value = Visit(self, node->value, scope, false);
            if (node->value->type == AST_COMPOUND) {
                node->value = getFirstInside((astCompound_t*)node->value);
                if (!node->value) {
                    printf("[Visitor]: TODO: trying to take the address of a what now?\n");
                    exit(1);
                }
            }

            if (node->value->type == AST_UNARY_OP && ((astUnaryOp_t*)node->value)->op == TOKEN_HASH) {
                printf("[Visitor]: TODO: can't get the address of an address\n");
                exit(1);
            }
                        
            if (node->value->type != AST_VARIABLE && !(node->value->type == AST_UNARY_OP && ((astUnaryOp_t*)node->value)->op == TOKEN_AT)) {
                printf("[Visitor]: TODO: can't get the address of an expression\n");
                exit(1);
            }

            node->base.dtypeInfo = node->value->dtypeInfo;
            node->base.dtypeInfo.ptrCount++;
            return (ast_t*) node;

        case TOKEN_AT:
            node->value = Visit(self, node->value, scope, false);
            if (node->value->type == AST_COMPOUND) {
                node->value = getFirstInside((astCompound_t*)node->value);
                if (!node->value) {
                    printf("[Visitor]: TODO: trying to dereference what now?\n");
                    exit(1);
                }
            }

            if (node->value->type == AST_UNARY_OP && ((astUnaryOp_t*)node->value)->op == TOKEN_HASH)
                return ((astUnaryOp_t*)node->value)->value;
            

            if (node->value->dtypeInfo.ptrCount < 1) {
                printf("[Visitor]: TODO: can't dereference a non pointer type\n");
                exit(1);
            }

            node->base.dtypeInfo = node->value->dtypeInfo;
            node->base.dtypeInfo.ptrCount--;
            return (ast_t*) node;

        case TOKEN_SIZEOF:
            // TODO: add support for structs
            break;
        default: break;
    }

    node->value = Visit(self, node->value, scope, false);
    return (ast_t*) node;
}

ast_t* VisitUnaryStatement(visitor_t* self, astUnaryStatement_t* node, scope_t* scope) {
    return (ast_t*) node;
}

ast_t* VisitConditionalStatement(visitor_t* self, astConditionalStatement_t* node, scope_t* scope) {
    return (ast_t*) node;
}
