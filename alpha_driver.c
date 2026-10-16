#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alpha_token.h"
#include "alpha_lex.h"

extern FILE *yyin;
extern int line_number;
extern YYSTYPE yylval;

const char* token_category(int type) {
    switch (type) {
        case IF: return "KEYWORD IF";
        case ELSE: return "KEYWORD ELSE";
        case WHILE: return "KEYWORD WHILE";
        case FOR: return "KEYWORD FOR";
        case FUNCTION: return "KEYWORD FUNCTION";
        case RETURN: return "KEYWORD RETURN";
        case TRUE: return "BOOLEAN TRUE";
        case FALSE: return "BOOLEAN FALSE";
        case NIL: return "NIL";
        case IDENT: return "IDENTIFIER";
        case INTCONST: return "INTEGER";
        case REALCONST: return "FLOAT";
        case OPERATOR: return "OPERATOR";
        case PUNCTUATION: return "PUNCTUATION";
        case STRING: return "STRING";
        case COMMENT_SINGLE: return "SINGLE LINE COMMENT";
        case COMMENT_MULTI_START: return "MULTILINE COMMENT START";
        case COMMENT_MULTI_END: return "MULTILINE COMMENT END";
	case COMMENT_NESTED_START: return "NESTED COMMENT START";
        case COMMENT_NESTED_END: return "NESTED COMMENT END";

        default: return "UNKNOWN";
    }
}

/** Escapes special characters in string output */
void escape_string(char* dest, const char* src, size_t size) {
    size_t i, j = 0;
    for (i = 0; src[i] && j < size - 1; i++) {
        if (src[i] == '\\' && src[i+1]) {  // Detect escape sequences
            i++;  // Move to escaped character
            switch (src[i]) {
                case 'n': dest[j++] = '\n'; break;  // Actual newline
                case 't': dest[j++] = '\t'; break;  // Actual tab
                case '"': dest[j++] = '\"'; break;  // Actual quote
                case '\\': dest[j++] = '\\'; break; // Actual backslash
                default: dest[j++] = '\\'; dest[j++] = src[i]; break; // Preserve unknown escapes
            }
        } else {
            dest[j++] = src[i];
        }
    }
    dest[j] = '\0';  // Terminate string properly
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file> [output_file]\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening input file");
        return 1;
    }
    yyin = file;

    FILE *output = (argc > 2) ? fopen(argv[2], "w") : stdout;
    if (!output) {
        perror("Error opening output file");
        fclose(file);
        return 1;
    }

    int token_count = 0;
    int token;
    char escaped_str[1024];

    while ((token = alpha_yylex(&yylval)) != 0) {
        const char *category = token_category(token);

        if (token == STRING || token == IDENT || token == OPERATOR || token == PUNCTUATION ||
            token == COMMENT_SINGLE || token == COMMENT_MULTI_START || token == COMMENT_MULTI_END 
		 || token == COMMENT_NESTED_START || token == COMMENT_NESTED_END) {
            
            escape_string(escaped_str, yylval.str_val ? yylval.str_val : "(null)", sizeof(escaped_str));

            fprintf(output, "%d: #%d \"%s\" %-24s <-%s\n", line_number, ++token_count,
                (strcmp(escaped_str, "..") == 0) ? ".." :
                (strcmp(escaped_str, ".") == 0) ? "." :
                (strcmp(escaped_str, "%") == 0) ? "%" :
                escaped_str,
                
                (strcmp(escaped_str, "..") == 0) ? "PUNCTUATION DOUBLE DOT" :
                (strcmp(escaped_str, ".") == 0) ? "PUNCTUATION DOT" :
                (strcmp(escaped_str, "%") == 0) ? "OPERATOR MODULO" :
                category, 

                (token == IDENT || token == STRING) ? "char*" : "enumerated");

            if (yylval.str_val) {  
                free(yylval.str_val);
                yylval.str_val = NULL;
            }
        } else if (token == INTCONST) {
            fprintf(output, "%d: #%d \"%d\" %-24s <-integer\n", line_number, ++token_count, yylval.int_val, category);
        } else if (token == REALCONST) {
            fprintf(output, "%d: #%d \"%s\" %-24s <-float\n", line_number, ++token_count, yylval.str_val, category);
            free(yylval.str_val);
            yylval.str_val = NULL;
        } else {
            fprintf(output, "%d: #%d \"%s\" %-24s <-enumerated\n", line_number, ++token_count, category, category);
        }
    }

    fclose(file);
    if (output != stdout) fclose(output);
    return 0;
}
