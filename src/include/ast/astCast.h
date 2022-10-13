#pragma once

#include "ast.h"

typedef struct astCast_t astCast_t;
struct astCast_t {
    ast_t base;

    dtypeInfo_t dtypeInfo;
    ast_t* expr;
};

void AstCast(astCast_t* self, dtypeInfo_t dtypeInfo, ast_t* expr);
astCast_t* newAstCast(dtypeInfo_t dtypeInfo, ast_t* expr);