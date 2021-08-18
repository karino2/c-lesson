#include "clesson.h"
#include "stack.h"
#include "parser.h"
#include <assert.h>

static int streq(char* s1, char* s2) {
    return strcmp(s1, s2) == 0;
}

void eval() {
    int ch = EOF;
    Token token = {
        LT_UNKNOWN,
        {0}
    };

    do {
        ch = parse_one(ch, &token);
        if (token.ltype != LT_UNKNOWN) {
            switch (token.ltype) {
            case LT_NUMBER: {
                StackElement element = { ET_NUMBER, {.number = token.u.number} };
                stack_push(&element);
                break;
            }
            case LT_EXECUTABLE_NAME:
                if (streq(token.u.name, "add")) {
                    StackElement e1, e2;
                    stack_pop(&e1);
                    stack_pop(&e2);
                    if (e1.type != ET_NUMBER || e2.type != ET_NUMBER) {
                        printf("add expects number operands, but got (%d, %d)\n", e1.type, e2.type);
                        break;
                    }

                    StackElement element = { ET_NUMBER, {.number = e1.u.number + e2.u.number} };
                    stack_push(&element);
                    break;
                }
            case LT_LITERAL_NAME: {
                StackElement element = { ET_LITERAL_NAME, {.name = token.u.name} };
                stack_push(&element);
                break;
            }
            case LT_SPACE:
            case LT_OPEN_CURLY:
            case LT_CLOSE_CURLY:
                break;
            default:
                printf("Unknown type %d\n", token.ltype);
                break;
            }
        }
    } while (ch != EOF);
}

static void verify_stack_pop_number_eq(int expects[], int nexpects) {
    for (int i = 0; i < nexpects; i++) {
        StackElement actual;
        stack_pop(&actual);

        assert(actual.type == ET_NUMBER);
        assert(actual.u.number == expects[i]);
    }
}

static void test_eval_num_one() {
    char* input = "123";
    int expects[1] = { 123 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_two() {
    char* input = "123 456";
    int expects[2] = { 456, 123 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 2);
}


static void test_eval_num_add() {
    char* input = "1 2 add";
    int expects[1] = { 3 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_add_many() {
    char* input = "1 2 3 add add 4 5 6 7 8 9 add add add add add add";
    int expects[1] = { 45 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_name_one() {
    char* input = "hoge";
    char* expect = "hoge";

    cl_getc_set_src(input);

    eval();

    StackElement actual;
    stack_pop(&actual);

    assert(actual.type == ET_LITERAL_NAME);
    assert(strcmp(actual.u.name, expect) == 0);
}

int main() {
    test_eval_num_one();
    test_eval_num_two();
    test_eval_num_add();
    test_eval_num_add_many();
    test_eval_name_one();

    return 0;
}