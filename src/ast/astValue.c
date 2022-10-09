#include "../include/ast/astValue.h"

void AstValue(astValue_t* self, astType_t astType, dtypeInfo_t dtypeInfo, char* value) {
    Ast(&self->base, astType);
    // self->base.Print = (void (*)(ast_t*, size_t)) AddressPrint;
    self->base.dtypeInfo = dtypeInfo;
    self->value = value;
}

astValue_t* newAstValue(astType_t astType, dtypeInfo_t dtypeInfo, char* value) {
    astValue_t* self = (astValue_t*) malloc(sizeof(astValue_t));
    if (!self) error_malloc(__func__);

    AstValue(self, astType, dtypeInfo, value);
    return self;
}
