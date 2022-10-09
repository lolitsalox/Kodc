#include "include/token.h"
#include "include/error.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void TokenPrint(token_t* self) {
    printf("(%s): %s\n", self->Str(self), self->value[0] == '\n' ? "\\n" : self->value);
}

const char* TokenStr(token_t* self) {
    size_t size = strlen(self->value) + strlen(tokenTypeToStr(self->type)) + 7;
    char* buf = (char*) malloc(size);
    sprintf_s(buf, size, "(%s): `%s`", tokenTypeToStr(self->type), self->value[0] == '\n' ? "\\n" : self->value);
    return buf;
}

void Token(token_t* self, char* value, tokenType_t type) {
    self->type = type;
    self->value = value;

    self->Print = TokenPrint;
    self->Str = TokenStr;
}

token_t* newToken(char* value, tokenType_t type) {
    token_t* self = calloc(1, sizeof(token_t));
    if (!self) {
        error_calloc(__func__);
    }
    
    Token(self, value, type);

    return self;
}

const char* tokenTypeToStr(tokenType_t type) {
    switch (type) {
        case TOKEN_ADD:             return          "TOKEN_ADD";
        case TOKEN_SUB:             return          "TOKEN_SUB";               
        case TOKEN_DIV:             return          "TOKEN_DIV";               
        case TOKEN_MUL:             return          "TOKEN_MUL";               
        case TOKEN_MOD:             return          "TOKEN_MOD";               
        case TOKEN_POW:             return          "TOKEN_POW";               
        case TOKEN_AND:             return          "TOKEN_AND";               
        case TOKEN_OR:              return          "TOKEN_OR";               
        case TOKEN_HAT:             return          "TOKEN_HAT";               
        case TOKEN_SHL:             return          "TOKEN_SHL";               
        case TOKEN_SHR:             return          "TOKEN_SHR";               
        case TOKEN_NOT:             return          "TOKEN_NOT";               
        case TOKEN_BOOL_NOT:        return          "TOKEN_BOOL_NOT";          
        case TOKEN_BOOL_EQ:         return          "TOKEN_BOOL_EQ";           
        case TOKEN_BOOL_NOTE:       return          "TOKEN_BOOL_NOTE";         
        case TOKEN_BOOL_LT:         return          "TOKEN_BOOL_LT";           
        case TOKEN_BOOL_GT:         return          "TOKEN_BOOL_GT";           
        case TOKEN_BOOL_LTE:        return          "TOKEN_BOOL_LTE";          
        case TOKEN_BOOL_GTE:        return          "TOKEN_BOOL_GTE";          
        case TOKEN_BOOL_AND:        return          "TOKEN_BOOL_AND";          
        case TOKEN_BOOL_OR:         return          "TOKEN_BOOL_OR";         
        case TOKEN_ID:              return          "TOKEN_ID";              
        case TOKEN_KEYWORD:         return          "TOKEN_KEYWORD";         
        case TOKEN_CHAR:            return          "TOKEN_CHAR";              
        case TOKEN_STRING:          return          "TOKEN_STRING";            
        case TOKEN_INT:             return          "TOKEN_INT";               
        case TOKEN_FLOAT:           return          "TOKEN_FLOAT";             
        case TOKEN_LPAREN:          return          "TOKEN_LPAREN";            
        case TOKEN_RPAREN:          return          "TOKEN_RPAREN";            
        case TOKEN_LBRACKET:        return          "TOKEN_LBRACKET";          
        case TOKEN_RBRACKET:        return          "TOKEN_RBRACKET";          
        case TOKEN_LBRACE:          return          "TOKEN_LBRACE";            
        case TOKEN_RBRACE:          return          "TOKEN_RBRACE";            
        case TOKEN_EQUALS:          return          "TOKEN_EQUALS";            
        case TOKEN_COMMA:           return          "TOKEN_COMMA";             
        case TOKEN_DOT:             return          "TOKEN_DOT";               
        case TOKEN_COLON:           return          "TOKEN_COLON";             
        case TOKEN_NAMESPACE:       return          "TOKEN_NAMESPACE";         
        case TOKEN_SEMI:            return          "TOKEN_SEMI";              
        case TOKEN_QUESTION:        return          "TOKEN_QUESTION";          
        case TOKEN_AT:              return          "TOKEN_AT";               
        case TOKEN_HASH:            return          "TOKEN_HASH";               
        case TOKEN_LINE_COMMENT:    return          "TOKEN_LINE_COMMENT";      
        case TOKEN_MULTILINE_COMMENT_START: return  "TOKEN_MULTILINE_COMMENT_START";   
        case TOKEN_MULTILINE_COMMENT_END:   return  "TOKEN_MULTILINE_COMMENT_END";     
        case TOKEN_POINTER:         return          "TOKEN_POINTER";           
        case TOKEN_ARROW:           return          "TOKEN_ARROW";             
        case TOKEN_BACKSLASH:       return          "TOKEN_BACKSLASH";         
        case TOKEN_NL:              return          "TOKEN_NL";               
        case TOKEN_EOF:             return          "TOKEN_EOF";               
        default:                    return          "TOKEN_UNKNOWN";
    }
}