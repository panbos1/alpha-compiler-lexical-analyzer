%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "alpha_token.h"

    YYSTYPE yylval;
    int line_number = 1;
    int nested_comment_level = 0;

    int yywrap(void) { return 1; }
%}

%x COMMENT

%%

".." { yylval.str_val = strdup(".."); return PUNCTUATION; }
"."  { yylval.str_val = strdup("."); return PUNCTUATION; }
"%"  { yylval.str_val = strdup("%"); return OPERATOR; }

"//".* { 
    yylval.str_val = strdup("//");
    return COMMENT_SINGLE;
}

"/*" { 
    nested_comment_level++;
    yylval.str_val = strdup("/*");

    if (nested_comment_level == 1) {
        BEGIN(COMMENT);
        return COMMENT_MULTI_START; // First-level multiline comment
    } else {
        return COMMENT_NESTED_START; // Nested comment inside another comment
    }
}

<COMMENT>"/*" { 
    nested_comment_level++;
    yylval.str_val = strdup("/*");
    return COMMENT_NESTED_START; // Return nested comment when found inside a multiline comment
}

<COMMENT>"*/" { 
    nested_comment_level--;
    yylval.str_val = strdup("*/");

    if (nested_comment_level == 0) {
        BEGIN(INITIAL);
        return COMMENT_MULTI_END; // Last closing comment
    } else {
        return COMMENT_NESTED_END; // Closing nested comment inside another comment
    }
}

<COMMENT>\n { line_number++; }

<COMMENT>[^*/\n]+ { /* Ignore all content inside comment */ }

<COMMENT>[*/] { /* Ignore individual '*' and '/' inside comments */ }

<COMMENT><<EOF>> {
    printf("Error: Unclosed comments exist!\n", line_number);
    exit(EXIT_FAILURE);
}

"if"        { yylval.str_val = strdup("if"); return IF; }
"else"      { yylval.str_val = strdup("else"); return ELSE; }
"while"     { yylval.str_val = strdup("while"); return WHILE; }
"for"       { yylval.str_val = strdup("for"); return FOR; }
"function"  { yylval.str_val = strdup("function"); return FUNCTION; }
"return"    { yylval.str_val = strdup("return"); return RETURN; }
"true"      { yylval.str_val = strdup("true"); return TRUE; }
"false"     { yylval.str_val = strdup("false"); return FALSE; }
"nil"       { yylval.str_val = strdup("nil"); return NIL; }

[0-9]+ {
    if (yytext == NULL) {  
        fprintf(stderr, "Error: yytext NULL, line %d\n", line_number);
        exit(EXIT_FAILURE);
    }
    char *endptr;
    errno = 0;
    long value = strtol(yytext, &endptr, 10);
    if (errno == ERANGE) {
        fprintf(stderr, "Error: Integer overflow/underflow at line %d: %s\n", line_number, yytext);
        exit(EXIT_FAILURE);
    }
    if (*endptr != '\0') {
        fprintf(stderr, "Error: Invalid integer at line %d: %s\n", line_number, yytext);
        exit(EXIT_FAILURE);
    }
    yylval.int_val = (int)value;  
    return INTCONST;
}

[0-9]+\.[0-9]+ { yylval.str_val = strdup(yytext); return REALCONST; }

"=="|"!="|">="|"<="|">"|"<"|"+"|"-"|"*"|"/"|"=" { 
    yylval.str_val = strdup(yytext); 
    return OPERATOR; 
}

[\{\}\[\]\(\);,:] { 
    yylval.str_val = strdup(yytext); 
    return PUNCTUATION; 
}

[a-zA-Z_][a-zA-Z0-9_]* { 
    yylval.str_val = strdup(yytext); 
    return IDENT; 
}

\"(\\.|[^"\\])*\" { 
    char* raw_string = strndup(yytext + 1, strlen(yytext) - 2);  // Remove surrounding quotes
    yylval.str_val = malloc(strlen(raw_string) + 1);  // Allocate enough space

    char *src = raw_string, *dest = yylval.str_val;
    while (*src) {
        if (*src == '\\' && *(src + 1)) {
            src++; // Skip the backslash and check next character
            switch (*src) {
                case 'n': *dest++ = '\n'; break; // Actual newline
                case 't': *dest++ = '\t'; break; // Actual tab
                case '\"': *dest++ = '\"'; break; // Quote
                case '\\': *dest++ = '\\'; break; // Backslash
                default: *dest++ = '\\'; *dest++ = *src; break; // Keep as is if unknown
            }
        } else {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';

    free(raw_string);
    return STRING;
}

\"(\\.|[^"\\])*  {
    printf("Illegal string (without final double quote) at line %d: %s\n", line_number, yytext);
}

[ \t]+       
\n           { line_number++; }

.            { printf("Unknown character: %s\n", yytext); }

%%

int alpha_yylex(void* ylval) {
    return yylex();
}


