#include "include/parser.h"
#include "include/error.h"

#include "include/ast/astAddress.h"
#include "include/ast/astAssignment.h"
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

#define SKIPNL while(self->token->type == TOKEN_NL) ParserEat(self, TOKEN_NL);

token_t* ParserEat(parser_t* self, tokenType_t type);

ast_t* ParseExpr(parser_t* self);
ast_t* ParseCompound(parser_t* self);
ast_t* ParseBlock(parser_t* self);
ast_t* ParseStatement(parser_t* self, statementType_t stype);

ast_t* ParseExprAssignment(parser_t* self);  // =
ast_t* ParseLogicalOr(parser_t* self);       // ||
ast_t* ParseLogicalAnd(parser_t* self);      // &&
ast_t* ParseBitwiseOr(parser_t* self);       // |
ast_t* ParseBitwiseXor(parser_t* self);      // ^
ast_t* ParseBitwiseAnd(parser_t* self);      // &
ast_t* ParseBoolEquals(parser_t* self);      // == !=
ast_t* ParseBoolGTLT(parser_t* self);        // < > <= >=
ast_t* ParseBitwiseSHLR(parser_t* self);     // << >>
ast_t* ParseAddSub(parser_t* self);          // + -
ast_t* ParseMDR(parser_t* self);             // * / %
ast_t* ParseSecond(parser_t* self);          // + - ! ~ cast @ # sizeof       
ast_t* ParseFirst(parser_t* self);           // from the right () [] . 
ast_t* ParseFactor(parser_t* self);          // values themselves

token_t* ParserEat(parser_t* self, tokenType_t type) {
    if (self->token->type != type)
        error_parser_unexpected_token_expected(self->token->Str(self->token), tokenTypeToStr(type));
    
    self->token = self->lexer->NextToken(self->lexer);
    return self->token;
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
        case STATEMENT_RETURN: return (ast_t*) newAstReturnStatement(ParseExpr(self));
        default:
            printf("[Parser]: TODO: still don't support %s", stype_to_str(stype));
            exit(1);
    }

    return NULL;
}

ast_t* ParseExpr(parser_t* self) {
    return ParseExprAssignment(self);
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

ast_t* ParseExprAssignment(parser_t* self) {
    ast_t* left = ParseLogicalOr(self);
    bool typeDefined = left->type == AST_VARIABLE ? ((astVariable_t*)left)->typeDefined : false;

    // TODO: add support for +=, *=, etc...
    if (self->token->type != TOKEN_EQUALS) return left;

    ParserEat(self, TOKEN_EQUALS);

    return (ast_t*)newAstAssignment(typeDefined, left, ParseLogicalOr(self));
}

ast_t* ParseLogicalOr(parser_t* self) {
    ast_t* left = ParseLogicalAnd(self);

    if (self->token->type == TOKEN_BOOL_OR) {
        left = (ast_t*) newAstBinOp(self->token->type, left, NULL);
        ParserEat(self, self->token->type);

        ((astBinOp_t*)left)->base.right = ParseLogicalOr(self);
    }

    return left;
}   

ast_t* ParseLogicalAnd(parser_t* self) {
    ast_t* left = ParseBitwiseOr(self);

    if (self->token->type == TOKEN_BOOL_AND) {
        left = (ast_t*) newAstBinOp(self->token->type, left, NULL);
        ParserEat(self, self->token->type);

        ((astBinOp_t*)left)->base.right = ParseLogicalAnd(self);
    }

    return left;
}   

ast_t* ParseBitwiseOr(parser_t* self) {
    ast_t* left = ParseBitwiseXor(self);

    if (self->token->type == TOKEN_OR) {
        left = (ast_t*) newAstBinOp(self->token->type, left, NULL);
        ParserEat(self, self->token->type);

        ((astBinOp_t*)left)->base.right = ParseBitwiseOr(self);
    }

    return left;
}   

ast_t* ParseBitwiseXor(parser_t* self) {
    ast_t* left = ParseBitwiseAnd(self);

    if (self->token->type == TOKEN_HAT) {
        left = (ast_t*) newAstBinOp(self->token->type, left, NULL);
        ParserEat(self, self->token->type);

        ((astBinOp_t*)left)->base.right = ParseBitwiseXor(self);
    }

    return left;
}   

ast_t* ParseBitwiseAnd(parser_t* self) {
    ast_t* left = ParseBoolEquals(self);

    if (self->token->type == TOKEN_AND) {
        left = (ast_t*) newAstBinOp(self->token->type, left, NULL);
        ParserEat(self, self->token->type);

        ((astBinOp_t*)left)->base.right = ParseBitwiseAnd(self);
    }

    return left;
}   

ast_t* ParseBoolEquals(parser_t* self) {
    ast_t* left = ParseBoolGTLT(self);

    if (
        self->token->type == TOKEN_BOOL_EQ || 
        self->token->type == TOKEN_BOOL_NOTE
    ) {
        left = (ast_t*) newAstBinOp(self->token->type, left, NULL);
        ParserEat(self, self->token->type);

        ((astBinOp_t*)left)->base.right = ParseBoolEquals(self);
    }

    return left;
}   

ast_t* ParseBoolGTLT(parser_t* self) {
    ast_t* left = ParseBitwiseSHLR(self);

    if (
        self->token->type == TOKEN_BOOL_GT ||
        self->token->type == TOKEN_BOOL_LT ||
        self->token->type == TOKEN_BOOL_GTE ||
        self->token->type == TOKEN_BOOL_LTE
    ) {
        left = (ast_t*) newAstBinOp(self->token->type, left, NULL);
        ParserEat(self, self->token->type);

        ((astBinOp_t*)left)->base.right = ParseBoolGTLT(self);
    }

    return left;
}   

ast_t* ParseBitwiseSHLR(parser_t* self) {
    ast_t* left = ParseAddSub(self);

    if (
        self->token->type == TOKEN_SHL ||
        self->token->type == TOKEN_SHR
    ) {
        left = (ast_t*) newAstBinOp(self->token->type, left, NULL);
        ParserEat(self, self->token->type);

        ((astBinOp_t*)left)->base.right = ParseBitwiseSHLR(self);
    }

    return left;
}   

ast_t* ParseAddSub(parser_t* self) {
    ast_t* left = ParseMDR(self);

    if (
        self->token->type == TOKEN_ADD ||
        self->token->type == TOKEN_SUB
    ) {
        left = (ast_t*) newAstBinOp(self->token->type, left, NULL);
        ParserEat(self, self->token->type);

        ((astBinOp_t*)left)->base.right = ParseAddSub(self);
    }

    return left;
}   

ast_t* ParseMDR(parser_t* self) {
    ast_t* left = ParseSecond(self);

    if (
        self->token->type == TOKEN_MUL ||
        self->token->type == TOKEN_DIV ||
        self->token->type == TOKEN_MOD
    ) {
        left = (ast_t*) newAstBinOp(self->token->type, left, NULL);
        ParserEat(self, self->token->type);

        ((astBinOp_t*)left)->base.right = ParseMDR(self);
    }

    return left;
}   

ast_t* ParseSecond(parser_t* self) {
    // Unary stuff like sizeof, +, -, @, #, ~, !
    astUnaryOp_t* ast = NULL;

    if (
        self->token->type == TOKEN_SIZEOF ||
        self->token->type == TOKEN_ADD ||
        self->token->type == TOKEN_SUB ||
        self->token->type == TOKEN_AT ||
        self->token->type == TOKEN_HASH ||
        self->token->type == TOKEN_NOT ||
        self->token->type == TOKEN_BOOL_NOT
    ) {
        ast = newAstUnaryOp(self->token->type, NULL);
        ParserEat(self, self->token->type);
        
        ast->value = ParseSecond(self);
    } else {
        return ParseFirst(self);
    }

    return (ast_t*) ast;
}   
   
ast_t* ParseFirst(parser_t* self) {
    ast_t* value = ParseFactor(self);

    switch (self->token->type) {
        case TOKEN_LPAREN: {
            ast_t* ast = ParseFactor(self);
            if (ast->type == AST_FUNCTION) {
                if (value->type != AST_VARIABLE) {
                    printf("[Parser]: wrong syntax for declaring a function\n");
                    exit(1);
                }
                ((astFunction_t*)ast)->name = ((astVariable_t*)value)->name;
                free(value);
                return ast;
            }

            if (ast->type != AST_COMPOUND) {
                printf("[Parser]: TODO: what are u calling lol\n");
                exit(1);
            }
            return (ast_t*) newAstCall(value, (astCompound_t*) ast);
            break;
        }

        case TOKEN_LBRACKET: {
            // TODO: array subscript
            break;
        }

        case TOKEN_DOT: {
            // TODO: struct access
            break;
        }

        default: break;
    }

    return value;
}   

ast_t* ParseFactor(parser_t* self) {
    ast_t* ast = NULL;
    switch (self->token->type) {
        case TOKEN_INT:
            ast = (ast_t*) newAstValue(AST_INT, ((dtypeInfo_t){DTYPE_I32, 0}), self->token->value);
            break;
        case TOKEN_FLOAT:
            ast = (ast_t*) newAstValue(AST_FLOAT, ((dtypeInfo_t){DTYPE_F32, 0}), self->token->value);
            break;
        case TOKEN_STRING:
            ast = (ast_t*) newAstValue(AST_STRING, ((dtypeInfo_t){DTYPE_I8, 1}), self->token->value);
            break;
        case TOKEN_ID: {

            statementType_t stype = str_to_stype(self->token->value);
            if (stype != STATEMENT_UNKNOWN) {
                ParserEat(self, self->token->type);
                return ParseStatement(self, stype);
            }

            ast = (ast_t*) newAstVariable(self->token->value);
            ParserEat(self, self->token->type);

            if (self->token->type == TOKEN_COLON) {
                ParserEat(self, self->token->type);
                
                // TODO: add ref support
                size_t ptrCount = 0;
                while (self->token->type == TOKEN_HASH) {
                    ptrCount++;
                    ParserEat(self, TOKEN_HASH);
                }
                
                dType_t dtype = strToDType(self->token->value);

                ParserEat(self, TOKEN_ID);

                ((astVariable_t*)ast)->base.dtypeInfo.dtype = dtype;
                ((astVariable_t*)ast)->base.dtypeInfo.ptrCount = ptrCount;
                ((astVariable_t*)ast)->typeDefined = true;
            }

            return ast;
        }
        case TOKEN_LPAREN: {
            ParserEat(self, TOKEN_LPAREN);
            astCompound_t* compound = newAstCompound();

            SKIPNL

            // if the list is not empty
            if (self->token->type != TOKEN_RPAREN) {
                ast_t* value = ParseExpr(self);

                if (value->type != AST_NOOP)
                    compound->children->PushBack(compound->children, value);
                else free(value);
            }

            SKIPNL
            
            // expr, expr, ...
            while (self->token->type == TOKEN_COMMA) {
                ParserEat(self, TOKEN_COMMA);

                SKIPNL

                ast_t* value = ParseExpr(self);
                if (value->type == AST_NOOP) { free(value); continue; }
                compound->children->PushBack(compound->children, value);
                
                SKIPNL
            }

            // )
            ParserEat(self, TOKEN_RPAREN);

            // Return type
            dtypeInfo_t dtypeInfo = { 0 };
            bool expectingFunction = false;

            if (self->token->type == TOKEN_POINTER) {
                ParserEat(self, TOKEN_POINTER);
                expectingFunction = true;

                while (self->token->type == TOKEN_HASH) {
                    dtypeInfo.ptrCount++;
                    ParserEat(self, TOKEN_HASH);
                }
                
                dtypeInfo.dtype = strToDType(self->token->value);
                ParserEat(self, TOKEN_ID);

            }
            
            SKIPNL

            // * Function
            if (self->token->type == TOKEN_LBRACE) {

                // TODO: check for invalid parameters in the visitor
                astFunction_t* func = newAstFunction();
                func->base.children = compound->children;

                free(func->body);
                func->body = (astCompound_t*) ParseBlock(self);
                if (expectingFunction)
                    func->returnTypeInfo = dtypeInfo;

                return (ast_t*) func;
            }
            
            return (ast_t*)compound;
        }
        case TOKEN_NL:
            ast = newAst(AST_NOOP);
            break;
        default: printf("[Parser]: TODO: unexpected token %s", self->token->Str(self->token)); exit(1);
    }

    ParserEat(self, self->token->type);
    return ast;
}