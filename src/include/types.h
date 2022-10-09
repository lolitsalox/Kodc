#pragma once

#include <stddef.h>

typedef enum dType_t {
    DTYPE_I8,
    DTYPE_I16,
    DTYPE_I32,
    DTYPE_I64,
    DTYPE_F32,
    DTYPE_U8,
    DTYPE_U16,
    DTYPE_U32,
    DTYPE_U64,
    DTYPE_F64,
    DTYPE_STRING,
    DTYPE_LIST,
    DTYPE_STRUCT,
    DTYPE_VOID,
    DTYPE_UNKNOWN,
} dType_t;

typedef struct dtypeInfo_t {
    dType_t dtype;    
    size_t ptrCount; // #i32, ##i32 -> 1, 2
} dtypeInfo_t;

char* dTypeToStr(dType_t type);
dType_t strToDType(const char* str);

size_t dtypeInfoSize(dtypeInfo_t type);