#include "clesson.h"
#include "stack.h"
#include "parser.h"
#include "dict.h"
#include <assert.h>

int streq(char* s1, char* s2) {
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
                }
                else if (streq(token.u.name, "def")) {
                    StackElement number, name;
                    stack_pop(&number);
                    stack_pop(&name);
                    if (number.type != ET_NUMBER || name.type != ET_LITERAL_NAME) {
                        printf("def expects number and literal name operands, but got (%d, %d)\n", number.type, name.type);
                        break;
                    }

                    dict_put(name.u.name, &number);
                }
                else {
                    // 変数名が dict に入っていたら値を stack に push する
                    StackElement e;
                    if (dict_get(token.u.name, &e)) {
                        stack_push(&e);
                    }
                }
                break;
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

static void test_eval_num_def() {
    char* input = "/abc 12 def abc";
    int expects[1] = { 12 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_name_one() {
    char* input = "/hoge";
    char* expect = "hoge";

    cl_getc_set_src(input);

    eval();

    StackElement actual;
    stack_pop(&actual);

    assert(actual.type == ET_LITERAL_NAME);
    assert(strcmp(actual.u.name, expect) == 0);
}

static void verify_dict_put_and_get(
    int ninput, char* input_keys[], StackElement input_elements[],
    char* target_key, int expect_found, int expect_value
) {
    for (int i = 0; i < ninput; i++) {
        dict_put(input_keys[i], &input_elements[i]);
    }

    StackElement actual_value;
    int actual_found = dict_get(target_key, &actual_value);

    assert(actual_found == expect_found);
    if (expect_found) {
        assert(actual_value.type == ET_NUMBER);
        assert(actual_value.u.number == expect_value);
    }
}

static void test_dict_no_element_name_not_found() {
    int ninput = 0;
    char* input_keys[] = {};
    StackElement input_elements[] = {};

    char* target_key = "foo";

    int expect_found = 0;
    int expect_value = -1;

    verify_dict_put_and_get(ninput, input_keys, input_elements, target_key, expect_found, expect_value);
}

static void test_dict_name_found() {
    int ninput = 1;
    char* input_keys[] = { "foo" };
    StackElement input_elements[] = { { ET_NUMBER, {.number = 42} } };

    char* target_key = "foo";

    int expect_found = 1;
    int expect_value = 42;

    verify_dict_put_and_get(ninput, input_keys, input_elements, target_key, expect_found, expect_value);
}

static void test_dict_name_not_found() {
    int ninput = 1;
    char* input_keys[] = { "foo" };
    StackElement input_elements[] = { { ET_NUMBER, {.number = 42} } };

    char* target_key = "bar";

    int expect_found = 0;
    int expect_value = -1;

    verify_dict_put_and_get(ninput, input_keys, input_elements, target_key, expect_found, expect_value);
}

static void test_dict_overwritten_name_found() {
    int ninput = 2;
    char* input_keys[] = { "foo", "foo" };
    StackElement input_elements[] = { { ET_NUMBER, {.number = 42} }, { ET_NUMBER, {.number = 420} } };

    char* target_key = "foo";

    int expect_found = 1;
    int expect_value = 420;

    verify_dict_put_and_get(ninput, input_keys, input_elements, target_key, expect_found, expect_value);
}

int main() {
    test_eval_num_one();
    test_eval_num_two();
    test_eval_num_add();
    test_eval_num_add_many();
    test_eval_num_def();
    test_eval_name_one();

    test_dict_no_element_name_not_found();
    test_dict_name_found();
    test_dict_name_not_found();
    test_dict_overwritten_name_found();

    return 0;
}