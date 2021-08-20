#include "clesson.h"
#include "stack.h"
#include "parser.h"
#include "dict.h"
#include <assert.h>

int streq(char* s1, char* s2) {
    return strcmp(s1, s2) == 0;
}

static int c_add(int n, int m) { return n + m; }
static int c_sub(int n, int m) { return n - m; }
static int c_mul(int n, int m) { return n * m; }
static int c_div(int n, int m) { return n / m; }

static void calc_op(char* name, int (*c_op)(int, int)) {
    StackElement e1, e2;
    stack_pop(&e1);
    stack_pop(&e2);
    if (e1.type != ET_NUMBER || e2.type != ET_NUMBER) {
        printf("%s expects number operands, but got (%d, %d)\n", name, e1.type, e2.type);
        exit(1);
    }

    StackElement element = { ET_NUMBER, {.number = c_op(e2.u.number, e1.u.number)} };
    stack_push(&element);
}

static void add_op() {
    calc_op("add", c_add);
}

static void sub_op() {
    calc_op("sub", c_sub);
}

static void mul_op() {
    calc_op("mul", c_mul);
}

static void div_op() {
    calc_op("div", c_div);
}

static void def_op() {
    StackElement number, name;
    stack_pop(&number);
    stack_pop(&name);
    if (number.type != ET_NUMBER || name.type != ET_LITERAL_NAME) {
        printf("def expects number and literal name operands, but got (%d, %d)\n", number.type, name.type);
        exit(1);
    }

    dict_put(name.u.name, &number);
}

static void register_primitives() {
    dict_put("add", &(StackElement){ ET_C_FUNC, { .cfunc = add_op } });
    dict_put("sub", &(StackElement){ ET_C_FUNC, { .cfunc = sub_op } });
    dict_put("mul", &(StackElement){ ET_C_FUNC, { .cfunc = mul_op } });
    dict_put("div", &(StackElement){ ET_C_FUNC, { .cfunc = div_op } });
    dict_put("def", &(StackElement){ ET_C_FUNC, { .cfunc = def_op } });
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
            case LT_EXECUTABLE_NAME: {
                StackElement e;
                if (!dict_get(token.u.name, &e)) {
                    printf("fail to resolve name %s\n", token.u.name);
                    exit(1);
                }

                if (e.type == ET_C_FUNC) {
                    // プリミティブ。事前に登録してある関数を実行
                    e.u.cfunc();
                }
                else {
                    // ユーザ定義の変数。変数が指す値を stack に push する
                    stack_push(&e);
                }
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

static void test_eval_num_sub() {
    char* input = "10 2 sub";
    int expects[1] = { 8 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_mul() {
    char* input = "8 9 mul";
    int expects[1] = { 72 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_div() {
    char* input = "27 3 div";
    int expects[1] = { 9 };

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

    // ユーザが put した element を掃除するために、一度全ての element を削除してからプリミティブを登録し直す
    dict_clear();
    register_primitives();
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

static void test_dict_name_found_when_hash_collides() {
    int ninput = 2;
    // この２つの key の hash が衝突するかは hash の実装に依存する。
    // このテストでは key の ASCII コードの合計値を hash 値とする実装を想定している。
    char* input_keys[] = { "foo", "oof" };
    StackElement input_elements[] = { { ET_NUMBER, {.number = 42} }, { ET_NUMBER, {.number = 24} } };

    char* target_key1 = "foo";

    int expect_found1 = 1;
    int expect_value1 = 42;

    verify_dict_put_and_get(ninput, input_keys, input_elements, target_key1, expect_found1, expect_value1);

    char* target_key2 = "oof";

    int expect_found2 = 1;
    int expect_value2 = 24;

    verify_dict_put_and_get(ninput, input_keys, input_elements, target_key2, expect_found2, expect_value2);
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
    register_primitives();

    test_eval_num_one();
    test_eval_num_two();
    test_eval_num_add();
    test_eval_num_add_many();
    test_eval_num_sub();
    test_eval_num_mul();
    test_eval_num_div();
    test_eval_num_def();
    test_eval_name_one();

    test_dict_no_element_name_not_found();
    test_dict_name_found();
    test_dict_name_found_when_hash_collides();
    test_dict_name_not_found();
    test_dict_overwritten_name_found();

    return 0;
}