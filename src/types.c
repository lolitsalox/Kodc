#include "include/types.h"

#include <string.h>

char* dTypeToStr(dType_t type) {
    switch (type) {
        case DTYPE_I8:        return "DTYPE_I8";
        case DTYPE_I16:       return "DTYPE_I16";
        case DTYPE_I32:       return "DTYPE_I32";
        case DTYPE_I64:       return "DTYPE_I64";
        case DTYPE_F32:       return "DTYPE_F32";
        case DTYPE_U8:        return "DTYPE_U8";
        case DTYPE_U16:       return "DTYPE_U16";
        case DTYPE_U32:       return "DTYPE_U32";
        case DTYPE_U64:       return "DTYPE_U64";
        case DTYPE_F64:       return "DTYPE_F64";
        case DTYPE_STRING:    return "DTYPE_STRING";
        case DTYPE_LIST:      return "DTYPE_LIST";
        case DTYPE_STRUCT:    return "DTYPE_STRUCT";
        default:              return "DTYPE_UNKNOWN";
    }
}

dType_t strToDType(const char* str) {
    if (!str) return DTYPE_UNKNOWN;

    switch (str[0]) {
        case 'i':
            if (strcmp(str, "i8") == 0)  return DTYPE_I8;
            if (strcmp(str, "i16") == 0) return DTYPE_I16;
            if (strcmp(str, "i32") == 0) return DTYPE_I32;
            if (strcmp(str, "i64") == 0) return DTYPE_I64;
            break;
        case 'u':
            if (strcmp(str, "u8") == 0)  return DTYPE_U8;
            if (strcmp(str, "u16") == 0) return DTYPE_U16;
            if (strcmp(str, "u32") == 0) return DTYPE_U32;
            if (strcmp(str, "u64") == 0) return DTYPE_U64;
            break;
        default: break;
    }

    if (strcmp(str, "string") == 0) return DTYPE_STRING;
    if (strcmp(str, "list") == 0)   return DTYPE_LIST;
    if (strcmp(str, "struct") == 0) return DTYPE_STRUCT;

    return DTYPE_UNKNOWN;
}

size_t dtypeSize(dType_t type) {
    switch (type) {
        case DTYPE_I8:
        case DTYPE_U8:        return 1;

        case DTYPE_I16:
        case DTYPE_U16:       return 2;

        case DTYPE_I32:
        case DTYPE_U32:
        case DTYPE_F32:       return 4;

        case DTYPE_I64:
        case DTYPE_U64:
        case DTYPE_F64:
        case DTYPE_STRING:
        case DTYPE_LIST:      return 8;
        default:              return 0;
    }
}