#include "include/parser.h"
#include "include/error.h"

#include "include/ast/astAddress.h"
#include "include/ast/astBinOp.h"
#include "include/ast/astCall.h"
#include "include/ast/astConditionalStatement.h"
#include "include/ast/astFunction.h"
#include "include/ast/astReturnStatement.h"
#include "include/ast/astUnaryOp.h"
#include "include/ast/astValue.h"
#include "include/ast/astVariable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token_t* ParserEat(parser_t* self, tokenType_t type);

ast_t* ParseAssignemnt(parser_t* self, ast_t* left);
ast_t* ParseId(parser_t* self);
ast_t* ParseExpr(parser_t* self);
ast_t* ParseCompound(parser_t* self);
ast_t* ParseList(parser_t* self, bool notLambda);
ast_t* ParseBlock(parser_t* self);
ast_t* ParseStatement(parser_t* self, statementType_t stype);
ast_t* ParseInt(parser_t* self);
ast_t* ParseAddress(parser_t* self);

token_t* ParserEat(parser_t* self, tokenType_t type) {
    if (self->token->type != type)
        error_parser_unexpected_token_expected(self->token->Str(self->token), tokenTypeToStr(type));
    
    self->token = self->lexer->NextToken(self->lexer);
    return self->token;
}


ast_t* ParseAssignemnt(parser_t* self, ast_t* left) {
    switch (self->token->type) {
        case TOKEN_EQUALS: {
            ParserEat(self, TOKEN_EQUALS);

            ast_t* right = ParseExpr(self);
            // Checking for invalid parameters
            if (right->type == AST_FUNCTION) {
                for (size_t i = 0; i < ((astFunction_t*)right)->base.children->size; ++i) {
                    ast_t* child = (ast_t*) (((astFunction_t*)right)->base.children->items[i]);
                    if (child->type != AST_VARIABLE) {
                        printf("[Parser]: TODO: invalid function definition\n");
                        exit(1);
                        // error_parser_invalid_func_def();
                    }
                }
                astBin_t* ast = newAstBin(AST_ASSIGNMENT, left, right);
                ast->base.dtypeInfo.dtype = DTYPE_U64;
                return (ast_t*) ast;
            }
            return (ast_t*) newAstBin(AST_ASSIGNMENT, left, right);
        }

        // TODO: add support for x -> y
        // case TOKEN_POINTER: {
        //     return (ast_t*) newASTAssignment(left, ParseAddress(self));
        // }

        case TOKEN_LPAREN: {
            // the return can be either a lambda function or a list of arguments 
            ast_t* ast = ParseList(self, true);
            switch (ast->type) {
                case AST_FUNCTION: {
                    // Checking for invalid parameters
                    for (size_t i = 0; i < ((astFunction_t*)ast)->base.children->size; ++i) {
                        ast_t* child = (ast_t*) (((astFunction_t*)ast)->base.children->items[i]);
                        if (child->type != AST_VARIABLE) {
                            printf("[Parser]: TODO: invalid function definition\n");
                            exit(1);
                            error_parser_invalid_func_def(((astFunction_t*)ast)->name);
                        }
                    }
                    return ast;
                }
                // Call
                case AST_COMPOUND: {
                    if (left->type != AST_VARIABLE) {
                        printf("[Parser]: TODO: invalid call\n");
                        exit(1);
                    }
                    return (ast_t*) newAstCall(((astVariable_t*)left)->name, (astCompound_t*) ast);;
                }
                default:
                    printf("How the hell did u get here?\n");
                    exit(1);
            }
            break;
        }
        default: break;
    }
    error_parser_unexpected_token_expected(tokenTypeToStr(self->token->type), "TOKEN_EQUALS OR TOKEN_LPAREN");
    return NULL;
}

ast_t* ParseId(parser_t* self) {
    char* name = (char*) calloc(strlen(self->token->value) + 1, sizeof(char));
    if (!name) error_calloc(__func__);
    strcpy(name, self->token->value);
    
    ParserEat(self, TOKEN_ID);

    // Checking if it's a statement
    statementType_t stype = str_to_stype(name);
    if (stype != STATEMENT_UNKNOWN)
        return ParseStatement(self, stype);

    // Assignment / Call
    if (self->token->type == TOKEN_EQUALS || self->token->type == TOKEN_LPAREN || self->token->type == TOKEN_POINTER)
        return ParseAssignemnt(self, (ast_t*) newAstVariable(name));

    if (self->token->type == TOKEN_COLON) {
        ParserEat(self, TOKEN_COLON);

        // if the dtype is unknown, in the visitor 
        // search for a structure definition using the context
        // TODO: add ref support
        size_t ptrCount = 0;
        while (self->token->type == TOKEN_HASH) {
            ptrCount++;
            ParserEat(self, TOKEN_HASH);
        }
        
        dType_t dtype = strToDType(self->token->value);

        ParserEat(self, TOKEN_ID);

        astVariable_t* left = newAstVariable(name);
        left->base.dtypeInfo.dtype = dtype;
        left->base.dtypeInfo.ptrCount = ptrCount;

        if (self->token->type == TOKEN_EQUALS)
            return ParseAssignemnt(self, (ast_t*) left);

        return (ast_t*)left;
    }

    if (self->token->type == TOKEN_EQUALS || self->token->type == TOKEN_LPAREN)
        return ParseAssignemnt(self, (ast_t*)newAstVariable(name));

    return (ast_t*)newAstVariable(name);
}


ast_t* ParseCompound(parser_t* self) {
    astCompound_t* compound = newAstCompound();

    while (self->token->type != TOKEN_EOF) {
        ast_t* value = ParseExpr(self);
        if (value->type == AST_NOOP) { free(value); continue; }
        compound->children->PushBack(compound->children, value);
    }

    return (ast_t*)compound;
}

ast_t* ParseList(parser_t* self, bool notLambda) {
    astCompound_t* compound = newAstCompound();

    ParserEat(self, TOKEN_LPAREN);
    while(self->token->type == TOKEN_NL)
        ParserEat(self, TOKEN_NL);

    if (self->token->type != TOKEN_RPAREN) {
        ast_t* value = ParseExpr(self);
        if (value->type != AST_NOOP)
            compound->children->PushBack(compound->children, value);
        else free(value);
    }

    while(self->token->type == TOKEN_NL)
        ParserEat(self, TOKEN_NL);
    
    while (self->token->type == TOKEN_COMMA) {
        ParserEat(self, TOKEN_COMMA);

        while(self->token->type == TOKEN_NL)
            ParserEat(self, TOKEN_NL);

        ast_t* value = ParseExpr(self);
        if (value->type == AST_NOOP) { free(value); continue; }
        compound->children->PushBack(compound->children, value);
        
        while(self->token->type == TOKEN_NL)
            ParserEat(self, TOKEN_NL);
    }
    while(self->token->type == TOKEN_NL)
        ParserEat(self, TOKEN_NL);
    ParserEat(self, TOKEN_RPAREN);

    bool expectingFunction = false;
    dType_t retType = DTYPE_UNKNOWN;
    size_t ptrCount = 0;

    if (self->token->type == TOKEN_POINTER) {
        ParserEat(self, TOKEN_POINTER);
        expectingFunction = true;

        while (self->token->type == TOKEN_HASH) {
            ptrCount++;
            ParserEat(self, TOKEN_HASH);
        }
        
        retType = strToDType(self->token->value);
        ParserEat(self, TOKEN_ID);

    }
    
    // * Skipping newlines
    while(self->token->type == TOKEN_NL)
        ParserEat(self, TOKEN_NL);

    // * Function
    if (self->token->type == TOKEN_LBRACE || self->token->type == TOKEN_POINTER) {

        if (!notLambda) {
            // Checking for invalid parameters
            for (size_t i = 0; i < compound->children->size; ++i) {
                if (((ast_t*)compound->children->items[i])->type != AST_VARIABLE) {
                    error_parser_invalid_func_def("<anonymous>");
                }
            }
        }

        astFunction_t* func = newAstFunction();
        func->base.children = compound->children;
        free(compound);
        
        func->body = (astCompound_t*) ParseBlock(self);
        if (expectingFunction) {
            func->returnTypeInfo.dtype = retType;
            func->returnTypeInfo.ptrCount = ptrCount;
        }

        return (ast_t*) func;
    }

    return (ast_t*)compound;
}

ast_t* ParseBlock(parser_t* self) {
    astCompound_t* compound = newAstCompound();
    ParserEat(self, TOKEN_LBRACE);

    while (self->token->type != TOKEN_RBRACE) {
        ast_t* value = ParseExpr(self);
        if (value->type == AST_NOOP) { free(value); continue; }
        compound->children->PushBack(compound->children, value);
    }

    ParserEat(self, TOKEN_RBRACE);

    return (ast_t*)compound;
}

ast_t* ParseStatement(parser_t* self, statementType_t stype) {
    switch (stype) {
        case STATEMENT_RETURN: {

            // astStatementReturn_t* ast = newASTStatementReturn();
            break;
        }
        default: break;
    }

    return NULL;
}

ast_t* ParseInt(parser_t* self) {
    return NULL;
    // astInt_t* ast = newASTInt(atoi(self->token->value));
    // ParserEat(self, TOKEN_INT);
    // return (ast_t*) ast;
}

// x = @#y
// x = @y
// x -> y
ast_t* ParseAddress(parser_t* self) {
    return NULL;
    // if (self->token->type == TOKEN_POINTER) {
    //     ParserEat(self, TOKEN_POINTER);
    //     ast_t* ast = ParseExpr(self);
    //     if (ast->type == AST_ADDRESS && ((astValue_t*)ast)->ref == true) {
    //         printf("[Parser]: TODO: you can't get the address of an address dumb dumb, its like &&x\n");
    //         exit(1);
    //     }
    //     return (ast_t*) newASTAddress(true, ast);
    // }

    // bool isRef = self->token->type == TOKEN_HASH; 
    // ParserEat(self, isRef ? TOKEN_HASH : TOKEN_AT);
    // ast_t* ast = ParseExpr(self);
    // if (ast->type == AST_ADDRESS) {
    //     // ?#
    //     if (((astValue_t*)ast)->ref) {
    //         // ## - illegal
    //         if (isRef) {
    //             printf("[Parser]: TODO: you can't get the address of an address dumb dumb, its like &&x\n");
    //             exit(1);
    //         }
    //         // @# - turning to normal
    //         return ((astValue_t*)ast)->value;
    //     }
    //     // ?@
    //     // #@ - turning to normal
    //     if (isRef)
    //         return ((astValue_t*)ast)->value;
    // }
    // // TODO: add this inside the visitor
    // // if (!isRef && ast->dtypeInfo.ptrCount == 0) {
    // //     printf("[Parser]: TODO: trying to dereference a non pointer type\n");
    // //     exit(1);
    // // }

    // if (ast->type == AST_ASSIGNMENT) {
    //     ((astAssignment_t*)ast)->deref = true;
    //     return ast;
    // }

    // astValue_t* addr = newASTAddress(isRef, ast);
    // addr->base.dtypeInfo = addr->value->dtypeInfo;
    // return (ast_t*) addr;    
}

ast_t* ParseExpr(parser_t* self) {
    switch (self->token->type) {
        case TOKEN_ID:      return ParseId(self);
        // case TOKEN_INT:     return ParseInt(self);
        case TOKEN_LPAREN:  return ParseList(self, false);
        case TOKEN_LBRACE:  return ParseBlock(self);
        // case TOKEN_HASH:
        // case TOKEN_AT:      return ParseAddress(self);
        case TOKEN_NL: { ParserEat(self, TOKEN_NL); return newAst(AST_NOOP); }
        default: error_parser_unexpected_token(tokenTypeToStr(self->token->type));
    }

    return newAst(AST_NOOP); 
}

ast_t* ParserParse(parser_t* self) {
    ast_t* root = ParseCompound(self);
    root->type = AST_ROOT;
    return root;
}

void Parser(parser_t* self, lexer_t* lexer) {
    self->lexer = lexer;
    self->token = lexer->NextToken(lexer);

    self->Parse = ParserParse;
}

parser_t* newParser(lexer_t* lexer) {
    parser_t* self = (parser_t*) malloc(sizeof(parser_t));
    if (!self) error_malloc(__func__);

    Parser(self, lexer);
    return self;
}
