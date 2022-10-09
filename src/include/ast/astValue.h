#pragma once

#include "ast.h"

typedef struct astValue_t astValue_t;
struct astValue_t {
    ast_t base;

    char* value;
};

void AstValue(astValue_t* self, astType_t astType, dtypeInfo_t dtypeInfo, char* value);
astValue_t* newAstValue(astType_t astType, dtypeInfo_t dtypeInfo, char* value);