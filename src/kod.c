#include "include/io.h"
#include "include/kod.h"
#include "include/list.h"
#include "include/parser.h"
#include "include/visitor.h"

#include <stdlib.h>
#include <stdio.h>

void kod_compile(char* src) {
    // TODO: Add a list of errors and warnings
    lexer_t* lexer = newLexer(src);
    parser_t* parser = newParser(lexer);
    
    ast_t* root = parser->Parse(parser);
    root->Print(root, 0);

    // visitor_t* visitor = newVisitor();
    // root = visitor->Visit(visitor, root, visitor->globalScope);

    // root->Print(root, 0);
    // Idea: stack capacity starts at 32 for the main function
    // each time when we increase the stack index, we make sure it's <= to the capacity
    // else, increase the capacity by 16 to align the stack like C

    // free(visitor);
    free(root);
    free(parser);
    free(lexer);
}

void kod_compile_file(const char* filename) {
    char* src = io_read(filename);
    kod_compile(src);
    free(src);
}