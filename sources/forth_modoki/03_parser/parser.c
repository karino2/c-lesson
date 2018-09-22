#include "clesson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

enum LexicalType {
    NUMBER,
    SPACE,
    COLON,
    SEMICOLON,
    PLUS,
    MINUS, 
    WORD,
    END_OF_FILE,
    UNKNOWN
};



struct Token {
    enum LexicalType ltype;
    union {
        int number;
        char onechar;
        char *word;
    } u;
};

#define WORD_SIZE 256

int parse_one(int prev_ch, struct Token *out_token) {
    out_token->ltype = UNKNOWN;
    return EOF;
}


void parser_print_all() {
    int ch = EOF;
    struct Token token = {
        UNKNOWN,
        {0}
    };

    do {
        ch = parse_one(ch, &token);
        if(token.ltype != UNKNOWN) {
            switch(token.ltype) {
                case NUMBER:
                    printf("num: %d\n", token.u.number);
                    break;
                case SPACE:
                    printf("space!\n");
                    break;
                case COLON:
                case SEMICOLON:
                case PLUS:
                case MINUS:
                    printf("special: %c\n", token.u.onechar);
                    break;
                case WORD:
                    printf("WORD: %s\n", token.u.word);
                    break;
                default:
                    printf("Unknown type %d\n", token.ltype);
                    break;
            }
        }
    }while(ch != EOF);
}





static void test_parse_one_number() {
    char *input = "123";
    int expect = 123;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);

    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == NUMBER);
    assert(expect == token.u.number);
}

static void test_parse_one_empty_should_return_END_OF_FILE() {
    char *input = "";
    int expect = END_OF_FILE;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);
    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == UNKNOWN);
}


static void unit_tests() {
    test_parse_one_empty_should_return_UNKNOWN();
    test_parse_one_number();
}

int main() {
    unit_tests();

    cl_getc_set_src("123 45 + 23 - someword");
    parser_print_all();
    return 1;
}