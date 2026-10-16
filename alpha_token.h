#ifndef ALPHA_TOKEN_H
#define ALPHA_TOKEN_H

typedef enum {
    KEYWORD,
    BOOLEAN,
    NIL,
    INTCONST,
    REALCONST,
    OPERATOR,
    PUNCTUATION,
    IDENT,
    STRING  
} token_type;

typedef union {
    int int_val;
    char* str_val;
} YYSTYPE;

extern YYSTYPE yylval;  

typedef struct {
    token_type type;
    char* value;
} alpha_token_t;

#define IF       256
#define ELSE     257
#define WHILE    258
#define FOR      259
#define FUNCTION 260
#define RETURN   261
#define TRUE     262
#define FALSE    263
#define NIL      264
#define INTCONST 265
#define REALCONST 266
#define OPERATOR 267
#define PUNCTUATION 268
#define IDENT    269
#define STRING   270  
#define COMMENT_SINGLE 271
#define COMMENT_MULTI_START 272
#define COMMENT_MULTI_END 273
#define COMMENT_NESTED_START 274
#define COMMENT_NESTED_END 275

#endif
