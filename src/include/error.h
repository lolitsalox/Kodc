#pragma once
#include "ast/ast.h"

#include <stddef.h>

void error_open(const char* filename);
void error_read(const char* filename);
void error_write(const char* filename);

void error_malloc(const char* what);
void error_calloc(const char* what);
void error_realloc(const char* what);

void error_usage(void);

void error_invalid_syntax(const char* name, const char* str, const char* msg, size_t index);

void error_parser_unexpected_token(const char* unexpected);
void error_parser_unexpected_token_expected(const char* unexpected, const char* expected);
void error_parser_invalid_func_def(const char* name);

void error_visitor_type(astType_t type);
void error_visitor_redefition(const char* name);
void error_visitor_undefined(const char* name);
void error_visitor_return_type_mismatch(const char* name, const char* funcReturnType, const char* returnType);
void error_visitor_multiple_return_types(const char* name, const char* typeOne, const char* typeTwo);