#include "include/error.h"

#include <stdio.h>
#include <stdlib.h>

void error_open(const char* filename) {
    printf("[%s]: couldn't open file.\n", filename);
    exit(1);
}

void error_read(const char* filename) {
    printf("[%s]: couldn't read file.\n", filename);
    exit(1);
}

void error_write(const char* filename) {
    printf("[%s]: couldn't write into file.\n", filename);
    exit(1);
}

void error_malloc(const char* what) {
    printf("[%s]: malloc() failed.\n", what);
    exit(1);
}

void error_calloc(const char* what) {
    printf("[%s]: calloc() failed.\n", what);
    exit(1);
}

void error_realloc(const char* what) {
    printf("[%s]: realloc() failed.\n", what);
    exit(1);
}

void error_usage(void) {
    printf("Usage: kod <filename>\n");
    exit(1);
}

void error_invalid_syntax(const char* name, const char* str, const char* msg, size_t index) {
    printf( "[%s]: Invalid syntax\n"
            "       %s\n"
            "        ", name, str);
    for (size_t i = 0; i < index; ++i)
        printf(" ");
    printf("^ %s\n", msg);
    exit(1);
    
}

void error_parser_unexpected_token(const char* unexpected) {
    printf("[Parser]: Unexpected token type %s.\n", unexpected);
    exit(1);
}

void error_parser_unexpected_token_expected(const char* unexpected, const char* expected) {
    printf("[Parser]: Unexpected token\n\tgot: %s, expected: %s.\n", unexpected, expected);
    exit(1);
}

void error_parser_invalid_func_def(const char* name) {
    printf("[Parser]: Invalid function definition: '%s'.\n", name);
    exit(1);
}

void error_visitor_type(astType_t type) {
    printf("[Visitor]: Don't know how to handle node type %s\n", astTypeToStr(type));
    exit(1);
}

void error_visitor_redefition(const char* name) {
    printf("[Visitor]: Redefinition of '%s'.\n", name);
    exit(1);
}

void error_visitor_undefined(const char* name) {
    printf("[Visitor]: '%s' is undefined.\n", name);
    exit(1);
}

void error_visitor_return_type_mismatch(const char* name, const char* funcReturnType, const char* returnType) {
    printf("[Visitor]: Return type mismatch at '%s'.\n\t", name);
    printf("Supposed to return %s, but returns %s.\n", funcReturnType, returnType);
    exit(1);
}

void error_visitor_multiple_return_types(const char* name, const char* typeOne, const char* typeTwo) {
    printf("[Visitor]: Multiple return types at '%s'.\n\t", name);
    printf("%s, %s", typeOne, typeTwo);
    exit(1);
}