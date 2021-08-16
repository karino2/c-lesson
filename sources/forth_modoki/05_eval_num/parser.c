#include "clesson.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define NAME_SIZE 256

static int isdigit(int c) {
    return '0' <= c && c <= '9';
}

static void parser_print_all() {
    int ch = EOF;
    Token token = {
        LT_UNKNOWN,
        {0}
    };

    do {
        ch = parse_one(ch, &token);
        if (token.ltype != LT_UNKNOWN) {
            switch (token.ltype) {
            case LT_NUMBER:
                printf("num: %d\n", token.u.number);
                break;
            case LT_SPACE:
                printf("space!\n");
                break;
            case LT_OPEN_CURLY:
                printf("Open curly brace '%c'\n", token.u.onechar);
                break;
            case LT_CLOSE_CURLY:
                printf("Close curly brace '%c'\n", token.u.onechar);
                break;
            case LT_EXECUTABLE_NAME:
                printf("EXECUTABLE_NAME: %s\n", token.u.name);
                break;
            case LT_LITERAL_NAME:
                printf("LITERAL_NAME: %s\n", token.u.name);
                break;
            default:
                printf("Unknown type %d\n", token.ltype);
                break;
            }
        }
    } while (ch != EOF);
}

int parse_one(int prev_ch, Token* out_token) {
    if (prev_ch == EOF) prev_ch = cl_getc();

    if (isdigit(prev_ch)) {
        int val = prev_ch - '0';

        int c;
        while ((c = cl_getc()) != EOF && isdigit(c)) {
            val *= 10;
            val += c - '0';
        }

        out_token->ltype = LT_NUMBER;
        out_token->u.number = val;

        return c;
    }
    else if (prev_ch == ' ') {
        int c;
        while ((c = cl_getc()) != EOF && c == ' ');

        out_token->ltype = LT_SPACE;
        return c;
    }
    else if ('A' <= prev_ch && prev_ch <= 'z') {
        char* name = malloc(NAME_SIZE * sizeof(char));
        name[0] = prev_ch;

        int c, i = 1;
        while ((c = cl_getc()) != EOF && c != ' ') {
            name[i++] = c;
        }
        name[i] = '\0';

        out_token->ltype = LT_EXECUTABLE_NAME;
        out_token->u.name = name;
        return c;
    }
    else if (prev_ch == '/') {
        char* name = malloc(NAME_SIZE * sizeof(char));

        int c, i = 0;
        while ((c = cl_getc()) != EOF && c != ' ') {
            name[i++] = c;
        }
        name[i] = '\0';

        out_token->ltype = LT_LITERAL_NAME;
        out_token->u.name = name;
        return c;
    }
    else if (prev_ch == '{') {
        out_token->ltype = LT_OPEN_CURLY;
        out_token->u.onechar = '{';
        return cl_getc();
    }
    else if (prev_ch == '}') {
        out_token->ltype = LT_CLOSE_CURLY;
        out_token->u.onechar = '}';
        return cl_getc();
    }
    else if (prev_ch == EOF) {
        out_token->ltype = LT_END_OF_FILE;
        return EOF;
    }
    else {
        out_token->ltype = LT_UNKNOWN;
        return EOF;
    }
}
