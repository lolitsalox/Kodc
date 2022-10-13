#pragma once

#include "ast/astFunction.h"

#include <stdbool.h>

typedef struct visitor_t visitor_t;

struct visitor_t {
    scope_t* globalScope;
    astFunction_t* currentFunction;

    ast_t* (*Visit) (visitor_t* self, ast_t* node, scope_t* scope, bool noerror);
};

void Visitor(visitor_t* self);
visitor_t* newVisitor();

// get in the visitor info about everything