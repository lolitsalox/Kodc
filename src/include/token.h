#pragma once

typedef struct token_t token_t;

typedef enum tokenType_t {
    TOKEN_ADD,                //  +
    TOKEN_SUB,                //  -
    TOKEN_DIV,                //  /
    TOKEN_MUL,                //  *
    TOKEN_MOD,                //  %
    TOKEN_POW,                //  ** NOT IMPLEMENTED IN THIS LEXER
    
    TOKEN_AND,                //  &
    TOKEN_OR,                 //  |
    TOKEN_HAT,                //  ^
    TOKEN_SHL,                //  <<
    TOKEN_SHR,                //  >>
    TOKEN_NOT,                //  ~
    
    TOKEN_BOOL_NOT,           //  !
    TOKEN_BOOL_EQ,            //  ==
    TOKEN_BOOL_NOTE,          //  !=
    TOKEN_BOOL_LT,            //  <
    TOKEN_BOOL_GT,            //  >
    TOKEN_BOOL_LTE,           //  <=
    TOKEN_BOOL_GTE,           //  >=
    TOKEN_BOOL_AND,           //  &&
    TOKEN_BOOL_OR,            //  ||
    TOKEN_ID,                 //  main x y foo
    TOKEN_KEYWORD,            //  while if
    TOKEN_SIZEOF,             //  sizeof
    
    TOKEN_CHAR,               //  'a'
    TOKEN_STRING,             //  "Hello world"
    TOKEN_INT,                //  5 6 456
    TOKEN_FLOAT,              //  6.9 7893.6   
    
    TOKEN_LPAREN,             //  (   
    TOKEN_RPAREN,             //  )              
    TOKEN_LBRACKET,           //  [              
    TOKEN_RBRACKET,           //  ]              
    TOKEN_LBRACE,             //  {          
    TOKEN_RBRACE,             //  }
    
    TOKEN_EQUALS,             //  =   
    TOKEN_COMMA,              //  ,  
    TOKEN_DOT,                //  .  
    TOKEN_COLON,              //  :  
    TOKEN_NAMESPACE,          //  ::  
    TOKEN_SEMI,               //  ;   
    TOKEN_QUESTION,           //  ?   
    TOKEN_AT,                 //  @
    TOKEN_HASH,               //  #
    TOKEN_LINE_COMMENT,       // //
    TOKEN_MULTILINE_COMMENT_START,     // /*
    TOKEN_MULTILINE_COMMENT_END,       // */
    TOKEN_POINTER,            //  ->
    TOKEN_ARROW,              //  =>
    TOKEN_BACKSLASH,          // 

    TOKEN_NL,                 //  New line
    TOKEN_EOF,                 //  The end of the file
    TOKEN_UNKNOWN,
} tokenType_t;

struct token_t {
    tokenType_t type;
    char* value;

    void (*Print) (token_t* self);
    const char* (*Str) (token_t* self); 
};

void Token(token_t* self, char* value, tokenType_t type);
token_t* newToken(char* value, tokenType_t type);

const char* tokenTypeToStr(tokenType_t type);