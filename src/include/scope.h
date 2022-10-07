#pragma once

#include "list.h"

typedef struct scope_t scope_t;

struct scope_t {
    scope_t* mom;
    list_t* variables; // ast*
    list_t* functions; // ast*
    list_t* children; // scopes
    int stackIndex;
    int paramSize;
};

void Scope(scope_t* self, scope_t* mom);
scope_t* newScope(scope_t* mom);