#include "include/parser.h"
#include "include/error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token_t* ParserEat(parser_t* self, tokenType_t type);

ast_t* ParseAssignemnt(parser_t* self, char* name);
ast_t* ParseId(parser_t* self);
ast_t* ParseExpr(parser_t* self);
ast_t* ParseCompound(parser_t* self);
ast_t* ParseList(parser_t* self, bool notLambda);
ast_t* ParseBlock(parser_t* self);
ast_t* ParseStatement(parser_t* self, statementType_t stype);
ast_t* ParseInt(parser_t* self);

token_t* ParserEat(parser_t* self, tokenType_t type) {
    if (self->token->type != type)
        error_parser_unexpected_token_expected(self->token->Str(self->token), tokenTypeToStr(type));
    
    self->token = self->lexer->NextToken(self->lexer);
    return self->token;
}


ast_t* ParseAssignemnt(parser_t* self, char* name) {
    switch (self->token->type) {
        case TOKEN_EQUALS: {
            ParserEat(self, TOKEN_EQUALS);

            ast_t* value = ParseExpr(self);
            // Checking for invalid parameters
            if (value->type == AST_FUNCTION) {
                for (size_t i = 0; i < ((astFunction_t*)value)->base.children->size; ++i) {
                    ast_t* child = (ast_t*) (((astFunction_t*)value)->base.children->items[i]);
                    if (child->type != AST_VARIABLE)
                        error_parser_invalid_func_def(name);
                }
                astAssignment_t* assign = newASTAssignment(name, value);
                assign->base.dtypeInfo.dtype = DTYPE_U64;
                return (ast_t*) assign;
            }
            return (ast_t*) newASTAssignment(name, value);
            break;
        }

        /*
        main()
        main() {}
        */
        case TOKEN_LPAREN: {
            // the return can be either a lambda function or a list of arguments 
            ast_t* ast = ParseList(self, true);
            switch (ast->type) {
                case AST_FUNCTION: {
                    ((astFunction_t*)ast)->name = name;

                    // Checking for invalid parameters
                    for (size_t i = 0; i < ((astFunction_t*)ast)->base.children->size; ++i) {
                        ast_t* child = (ast_t*) (((astFunction_t*)ast)->base.children->items[i]);
                        if (child->type != AST_VARIABLE)
                            error_parser_invalid_func_def(name);
                    }
                    return ast;
                }
                // Call
                case AST_COMPOUND: {
                    astCall_t* call = newASTCall(name, (astCompound_t*) ast);
                    return (ast_t*) call;
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
    if (!name)
        error_calloc(__func__);
    strcpy(name, self->token->value);
    
    ParserEat(self, TOKEN_ID);

    // Checking if it's a statement
    statementType_t stype = strToStatementType(name);
    if (stype != STATEMENT_UNKNOWN) {
        return ParseStatement(self, stype);
    }

    // Assignment / Call
    if (self->token->type == TOKEN_EQUALS || self->token->type == TOKEN_LPAREN)
        return ParseAssignemnt(self, name);

    /*
    class abc<T> {
        x: T
        abc(x: T) {
            self.x = x
        }
    }

    obj: abc<int> = abc<int>(5)
    print(obj.x) // outputs 5

    x: list<string> = ["main", "hello", "yuk"]
    */

    if (self->token->type == TOKEN_COLON) {
        ParserEat(self, TOKEN_COLON);

        // if the dtype is unknown, in the visitor 
        // search for a structure definition using the context
        char* dtStr = self->token->value;
        dType_t firstType = strToDType(dtStr);
        dType_t dtype = firstType;

        ParserEat(self, TOKEN_ID);

        int listCount = 0;
        dType_t typeOfList = DTYPE_UNKNOWN;

        // list<list<string>>
        if (self->token->type == TOKEN_BOOL_LT) {
            size_t num = 0; // we'll use it to compare the number of < and >

            while (self->token->type == TOKEN_BOOL_LT) {
                if (dtype != DTYPE_LIST) {
                    char* buf = (char*) malloc(strlen(dtStr) + 7);
                    sprintf_s(buf, strlen(dtStr) + 7, "%s<...>", dtStr);
                    error_invalid_syntax("Parser", buf, "<typename> is only supported by a list", strlen(dtStr) - 1);
                }

                ParserEat(self, TOKEN_BOOL_LT);

                dtStr = self->token->value;
                dtype = strToDType(dtStr);

                typeOfList = dtype;
                listCount++;
                ParserEat(self, TOKEN_ID);

            }
            while (listCount != num) {
                if (self->token->type == TOKEN_SHR) {
                    num += 2;
                    ParserEat(self, TOKEN_SHR);
                    continue;
                }
                num++;
                ParserEat(self, TOKEN_BOOL_GT);
            }
        }

        if (self->token->type == TOKEN_EQUALS) {
            astAssignment_t* assign = (astAssignment_t*) ParseAssignemnt(self, name);
            assign->base.dtypeInfo.dtype = firstType;
            assign->base.dtypeInfo.typeOfList = typeOfList;
            assign->base.dtypeInfo.listCount = listCount;
            return (ast_t*) assign;
        }

        astVariable_t* var = newASTVariable(name);
        var->base.dtypeInfo.dtype = firstType;
        var->base.dtypeInfo.typeOfList = typeOfList;
        var->base.dtypeInfo.listCount = listCount;
        return (ast_t*) var;
    }

    if (self->token->type == TOKEN_EQUALS || self->token->type == TOKEN_LPAREN)
        return ParseAssignemnt(self, name);

    astVariable_t* var = newASTVariable(name);
    return (ast_t*)var;
}

ast_t* ParseExpr(parser_t* self) {
    switch (self->token->type) {
        case TOKEN_ID: return ParseId(self);
        case TOKEN_INT: return ParseInt(self);
        case TOKEN_LPAREN: return ParseList(self, false);
        case TOKEN_LBRACE: return ParseBlock(self);
        case TOKEN_NL: { ParserEat(self, TOKEN_NL); return newAST(AST_NOOP); }
        default: error_parser_unexpected_token(tokenTypeToStr(self->token->type));
    }

    return newAST(AST_NOOP); 
}

ast_t* ParseCompound(parser_t* self) {
    astCompound_t* compound = newASTCompound();

    while (self->token->type != TOKEN_EOF) {
        ast_t* value = ParseExpr(self);
        if (value->type == AST_NOOP) { free(value); continue; }
        compound->children->PushBack(compound->children, value);
    }

    return (ast_t*)compound;
}

ast_t* ParseList(parser_t* self, bool notLambda) {
    astCompound_t* compound = newASTCompound();

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
    int listCount = 0;
    dType_t typeOfList = DTYPE_UNKNOWN;

    if (self->token->type == TOKEN_POINTER) {
        ParserEat(self, TOKEN_POINTER);
        expectingFunction = true;

        char* dtStr = self->token->value;
        retType = strToDType(dtStr);
        dType_t dtype = retType;

        ParserEat(self, TOKEN_ID);

        // list<list<string>>
        if (self->token->type == TOKEN_BOOL_LT) {
            size_t num = 0; // we'll use it to compare the number of < and >

            while (self->token->type == TOKEN_BOOL_LT) {
                if (dtype != DTYPE_LIST) {
                    char* buf = (char*) malloc(strlen(dtStr) + 7);
                    sprintf_s(buf, strlen(dtStr) + 7, "%s<...>", dtStr);
                    error_invalid_syntax("Parser", buf, "<typename> is only supported by a list", strlen(dtStr) - 1);
                }

                ParserEat(self, TOKEN_BOOL_LT);

                dtStr = self->token->value;
                dtype = strToDType(dtStr);

                typeOfList = dtype;
                listCount++;
                ParserEat(self, TOKEN_ID);

            }
            while (listCount != num) {
                if (self->token->type == TOKEN_SHR) {
                    num += 2;
                    ParserEat(self, TOKEN_SHR);
                    continue;
                }
                num++;
                ParserEat(self, TOKEN_BOOL_GT);
            }
        }


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

        astFunction_t* func = newASTFunction();
        func->base.children = compound->children;
        free(compound);
        
        func->value = (astCompound_t*) ParseBlock(self);
        if (expectingFunction) {
            func->returnTypeInfo.dtype = retType;
            func->returnTypeInfo.typeOfList = typeOfList;
            func->returnTypeInfo.listCount = listCount;
        }

        return (ast_t*) func;
    }

    return (ast_t*)compound;
}

ast_t* ParseBlock(parser_t* self) {
    astCompound_t* compound = newASTCompound();
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
    astInt_t* ast = newASTInt(atoi(self->token->value));
    ParserEat(self, TOKEN_INT);
    return (ast_t*) ast;
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
    if (!self)
        error_malloc(__func__);

    Parser(self, lexer);
    return self;
}
