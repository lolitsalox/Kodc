#include "include/scope.h"
#include "include/error.h"

#include <stdlib.h>

void Scope(scope_t* self, scope_t* mom) {
    self->mom = mom;
    self->variables = newList(sizeof(ast_t*));
    self->functions = newList(sizeof(ast_t*));
    self->children  = newList(sizeof(scope_t*));
    self->stackIndex = 0;
    self->paramSize = 0;
}

scope_t* newScope(scope_t* mom) {
    scope_t* self = (scope_t*) malloc(sizeof(scope_t));
    if (!self)
        error_malloc(__func__);

    Scope(self, mom);
    return self;
}