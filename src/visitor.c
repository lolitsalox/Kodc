#include "include/visitor.h"
#include "include/types.h"
#include "include/error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: understand from parsing the function, what type it should return 
// TODO: (find the return statements and check if they all return the same type)

astVariable_t* lookupVar(scope_t* scope, char* name) {
    if (scope->mom) {
        astVariable_t* var = lookupVar(scope->mom, name);
        if (var) return var;
    }

    for (size_t i = 0; i < scope->variables->size; ++i) {
        if (strcmp(((astVariable_t*)scope->variables->items[i])->name, name) == 0)
            return (astVariable_t*) scope->variables->items[i];
    }
    return NULL;
}

ast_t* VisitorVisitAssignment(visitor_t* self, astAssignment_t* node, scope_t* scope);
ast_t* VisitorVisitFunction(visitor_t* self, astFunction_t* node, scope_t* scope);
ast_t* VisitorVisitCompound(visitor_t* self, astCompound_t* node, scope_t* scope);
ast_t* VisitorVisitVariable(visitor_t* self, astVariable_t* node, scope_t* scope);
ast_t* VisitorVisitCall(visitor_t* self, astCall_t* node, scope_t* scope);
ast_t* VisitorVisit(visitor_t* self, ast_t* node, scope_t* scope);


ast_t* VisitorVisitAssignment(visitor_t* self, astAssignment_t* node, scope_t* scope) {
    astVariable_t* var = lookupVar(scope, node->name);
    if (var) {
        if (!scope->mom) // not var:dtype = or in global scope
            error_visitor_redefition(node->name);
        
        if (node->base.dtypeInfo.dtype != DTYPE_UNKNOWN && (
            node->base.dtypeInfo.dtype      != var->base.dtypeInfo.dtype || 
            node->base.dtypeInfo.typeOfList != var->base.dtypeInfo.typeOfList ||
            node->base.dtypeInfo.listCount  != var->base.dtypeInfo.listCount)) {
            printf( "[Visitor]: TODO: Changing the type of the variable\n"
                    "\t%s: %s -> %s: %s\n", 
                    node->name,
                    dTypeToStr(var->base.dtypeInfo.dtype), 
                    node->name, 
                    dTypeToStr(node->base.dtypeInfo.dtype)
            );
            exit(1);
        } 
        node->base.dtypeInfo = var->base.dtypeInfo;

        node->value = self->Visit(self, (ast_t*) node->value, scope);

        // Conflicting dtypes
        if (node->base.dtypeInfo.dtype      != node->value->dtypeInfo.dtype || 
            node->base.dtypeInfo.typeOfList != node->value->dtypeInfo.typeOfList ||
            node->base.dtypeInfo.listCount  != node->value->dtypeInfo.listCount) {
            printf( "[Visitor]: TODO: Incompatible conversion "
                    "assigning to '%s' from '%s'\n", 
                    dTypeToStr(node->base.dtypeInfo.dtype),
                    dTypeToStr(node->value->dtypeInfo.dtype));
            exit(1);
        }

        return (ast_t*) node;
    }


    var = newASTVariable(node->name);
    var->base.dtypeInfo = node->base.dtypeInfo;
    node->value = self->Visit(self, (ast_t*) node->value, scope);

    if (var->base.dtypeInfo.dtype == DTYPE_UNKNOWN) {
        // like the keyword auto assuming the type is whatever the expr gives
        var->base.dtypeInfo = node->value->dtypeInfo;
        node->base.dtypeInfo = node->value->dtypeInfo;
    } else {
        // Conflicting dtypes
        if (var->base.dtypeInfo.dtype      != node->value->dtypeInfo.dtype || 
            var->base.dtypeInfo.typeOfList != node->value->dtypeInfo.typeOfList ||
            var->base.dtypeInfo.listCount  != node->value->dtypeInfo.listCount) {
            printf( "[Visitor]: TODO: Incompatible conversion "
                    "assigning to '%s' from '%s'\n", 
                    dTypeToStr(node->base.dtypeInfo.dtype),
                    dTypeToStr(node->value->dtypeInfo.dtype));
            exit(1);
        }
    }

    if (scope->mom) {
        scope->stackIndex -= dtypeSize(var->base.dtypeInfo.dtype);
        var->stackIndex = scope->stackIndex;
    }

    scope->variables->PushBack(scope->variables, var);

    return (ast_t*) node;
}

ast_t* VisitorVisitFunction(visitor_t* self, astFunction_t* node, scope_t* scope) {
    node->scope = newScope(scope);
    self->currentFunction = node;

    int tSp = 16; // rip, rbp
    for (size_t i = 0; i < node->base.children->size; ++i) {
        astVariable_t* child = (astVariable_t*) node->base.children->items[i];
        node->base.children->items[i] = self->Visit(self, (ast_t*)child, node->scope);
        child = node->base.children->items[i];

        node->scope->variables->PushBack(node->scope->variables, child);
        child->stackIndex = tSp;
        tSp += dtypeSize(child->base.dtypeInfo.dtype);
        
    }
    node->scope->paramSize = tSp - 16; 

    scope->variables->PushBack(node->scope->variables, node);

    for (size_t i = 0; i < node->value->children->size; ++i) {
        ast_t* child = (ast_t*) node->value->children->items[i];
        node->value->children->items[i] = self->Visit(self, child, node->scope);
        child = node->value->children->items[i];
    }

    // TODO: Verify return type


    // * To check the allocations, uncomment
    printf("function %s:\n", node->name);
    printf("Need to allocate %d bytes for the params\nAnd %d bytes for the function stack frame\n", node->scope->paramSize, -node->scope->stackIndex);
    for (int i = 0; i < node->scope->variables->size; ++i) {
        if (((ast_t*)node->scope->variables->items[i])->type != AST_VARIABLE) continue;
        int si = ((astVariable_t*)node->scope->variables->items[i])->stackIndex;
        printf("#%d: '%s' [rbp %c %d]\n", i,  ((astVariable_t*)node->scope->variables->items[i])->name, si > 0 ? '+' : '-', abs(si));
    }
    printf("\n");
    self->currentFunction = node;

    if (strcmp(node->name, "anonymous") == 0) {
        printf("it's anonymous");
        return (ast_t*) node;
    }
    scope->variables->PushBack(scope->variables, node);
    return (ast_t*) node;
}

ast_t* VisitorVisitVariable(visitor_t* self, astVariable_t* node, scope_t* scope) {
    ast_t* var = (ast_t*)lookupVar(scope, node->name);
    if (var) return var;
    return (ast_t*) node;
}

ast_t* VisitorVisitCall(visitor_t* self, astCall_t* node, scope_t* scope) {
    astFunction_t* func = (astFunction_t*)lookupVar(scope, node->name);
    if (!func) {
        // Search for a builtin
        if (strcmp(node->name, "return") == 0) {
            for (size_t i = 0; i < node->value->children->size; ++i) {
                ast_t* child = (ast_t*) node->value->children->items[i];
                node->value->children->items[i] = self->Visit(self, child, scope);
                child = node->value->children->items[i];
            }
            if (!self->currentFunction) {
                printf("[Visitor]: Returning at global scope\n");
                exit(1);
            }
            

            if (node->value->children->size == 1) {
                if (self->currentFunction->returnTypeInfo.dtype == DTYPE_UNKNOWN) {
                    self->currentFunction->returnTypeInfo = ((ast_t*)node->value->children->items[0])->dtypeInfo;
                    return (ast_t*) node;
                }
                if (self->currentFunction->returnTypeInfo.dtype != ((ast_t*)node->value->children->items[0])->dtypeInfo.dtype ||
                    self->currentFunction->returnTypeInfo.typeOfList != ((ast_t*)node->value->children->items[0])->dtypeInfo.typeOfList ||
                    self->currentFunction->returnTypeInfo.listCount != ((ast_t*)node->value->children->items[0])->dtypeInfo.listCount
                ) {
                    error_visitor_return_type_mismatch(
                        self->currentFunction->name, 
                        dTypeToStr(self->currentFunction->returnTypeInfo.dtype), 
                        dTypeToStr(((ast_t*)node->value->children->items[0])->dtypeInfo.dtype)
                    );
                }
                return (ast_t*) node;
            }


            
            return (ast_t*) node;
        }
        
        error_visitor_undefined(node->name);
    }

    for (size_t i = 0; i < node->value->children->size; ++i) {
        ast_t* child = (ast_t*) node->value->children->items[i];
        node->value->children->items[i] = self->Visit(self, child, scope);
        child = node->value->children->items[i];
    }

    node->base.dtypeInfo = func->returnTypeInfo;

    return (ast_t*) node;
}

ast_t* VisitorVisitCompound(visitor_t* self, astCompound_t* node, scope_t* scope) {
    for (size_t i = 0; i < ((astCompound_t*)node)->children->size; ++i)
        self->Visit(self, node->children->items[i], scope);
    
    return (ast_t*) node;
}
ast_t* VisitorVisit(visitor_t* self, ast_t* node, scope_t* scope) {
    switch (node->type) {
        case AST_ROOT:
        case AST_COMPOUND:      return VisitorVisitCompound(self, (astCompound_t*) node, scope);
        case AST_FUNCTION:      return VisitorVisitFunction(self, (astFunction_t*) node, scope);
        case AST_ASSIGNMENT:    return VisitorVisitAssignment(self, (astAssignment_t*) node, scope);
        case AST_VARIABLE:      return VisitorVisitVariable(self, (astVariable_t*) node, scope);
        case AST_CALL:          return VisitorVisitCall(self, (astCall_t*) node, scope);
        case AST_INT:           return node;
        default: error_visitor_type(node->type);
    }

    return NULL;
}

void Visitor(visitor_t* self) {
    self->Visit = VisitorVisit;
    self->globalScope = newScope(NULL);
    self->currentFunction = NULL;    
}

visitor_t* newVisitor() {
    visitor_t* self = (visitor_t*) malloc(sizeof(visitor_t));
    if (!self)
        error_malloc(__func__);

    Visitor(self);
    return self;
}
