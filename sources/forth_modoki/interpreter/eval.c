#include "clesson.h"
#include "stack.h"
#include "parser.h"
#include "dict.h"
#include <assert.h>

#define MAX_NAME_OP_NUMBERS 256

static int c_add(int n, int m) { return n + m; }
static int c_sub(int n, int m) { return n - m; }
static int c_mul(int n, int m) { return n * m; }
static int c_div(int n, int m) { return n / m; }
static int c_eq(int n, int m) { return n == m; }
static int c_neq(int n, int m) { return n != m; }
static int c_gt(int n, int m) { return n > m; }
static int c_ge(int n, int m) { return n >= m; }
static int c_lt(int n, int m) { return n < m; }
static int c_le(int n, int m) { return n <= m; }

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

static void add_op() { calc_op("add", c_add); }
static void sub_op() { calc_op("sub", c_sub); }
static void mul_op() { calc_op("mul", c_mul); }
static void div_op() { calc_op("div", c_div); }
static void eq_op() { calc_op("eq", c_eq); }
static void neq_op() { calc_op("neq", c_neq); }
static void gt_op() { calc_op("gt", c_gt); }
static void ge_op() { calc_op("ge", c_ge); }
static void lt_op() { calc_op("lt", c_lt); }
static void le_op() { calc_op("le", c_le); }

static void pop_op() {
    StackElement e;
    stack_pop(&e);
}

static void exch_op() {
    StackElement e1, e2;
    stack_pop(&e1);
    stack_pop(&e2);
    stack_push(&e1);
    stack_push(&e2);
}

static void dup_op() {
    StackElement e;
    stack_pop(&e);
    stack_push(&e);
    stack_push(&e);
}

static void def_op() {
    StackElement value, key;
    stack_pop(&value);
    stack_pop(&key);
    if ((value.type != ET_NUMBER && value.type != ET_EXECUTABLE_ARRAY) || key.type != ET_LITERAL_NAME) {
        printf("def expects number or executable array as value and literal name as key, but got (%d, %d)\n", value.type, key.type);
        exit(1);
    }

    dict_put(key.u.name, &value);
}

static void register_primitive(char* key, void (*op)()) {
    dict_put(key, &(StackElement){ ET_C_FUNC, { .cfunc = op } });
}

static void register_primitives() {
    register_primitive("add", add_op);
    register_primitive("sub", sub_op);
    register_primitive("mul", mul_op);
    register_primitive("div", div_op);
    register_primitive("eq", eq_op);
    register_primitive("neq", neq_op);
    register_primitive("gt", gt_op);
    register_primitive("ge", ge_op);
    register_primitive("lt", lt_op);
    register_primitive("le", le_op);

    register_primitive("pop", pop_op);
    register_primitive("exch", exch_op);
    register_primitive("dup", dup_op);

    register_primitive("def", def_op);
}

static void compile_exec_array(StackElement* out_element) {
    int ch = EOF;
    Token token = {
        LT_UNKNOWN,
        {0}
    };

    int i = 0;
    StackElement tmp[MAX_NAME_OP_NUMBERS];

    do {
        ch = parse_one(ch, &token);
        switch (token.ltype) {
        case LT_NUMBER:
            tmp[i++] = (StackElement){ ET_NUMBER, {.number = token.u.number} };
            break;
        case LT_EXECUTABLE_NAME:
            tmp[i++] = (StackElement){ ET_EXECUTABLE_NAME, {.name = token.u.name} };
            break;
        case LT_LITERAL_NAME:
            tmp[i++] = (StackElement){ ET_LITERAL_NAME, {.name = token.u.name} };
            break;
        case LT_OPEN_CURLY: {
            StackElement e;
            compile_exec_array(&e);
            tmp[i++] = e;
            break;
        }
        case LT_CLOSE_CURLY:
        case LT_SPACE:
            break;
        default:
            break;
        }
    } while (token.ltype != EOF && token.ltype != LT_CLOSE_CURLY);

    StackElementArray* array = malloc(sizeof(StackElementArray) + sizeof(StackElement) * i);
    array->len = i;
    memcpy(array->elements, tmp, sizeof(StackElement) * i);
    *out_element = (StackElement){ ET_EXECUTABLE_ARRAY, {.byte_codes = array} };
}

static void eval_exec_array(StackElementArray* exec_array) {
    for (int i = 0; i < exec_array->len; i++) {
        StackElement e = exec_array->elements[i];
        switch (e.type) {
        case ET_NUMBER:
        case ET_LITERAL_NAME:
            stack_push(&e);
            break;
        case ET_EXECUTABLE_NAME: {
            StackElement e_name;
            if (!dict_get(e.u.name, &e_name)) {
                printf("fail to resolve name %s\n", e.u.name);
                exit(1);
            }

            if (e_name.type == ET_C_FUNC) {
                // プリミティブ。事前に登録してある関数を実行
                e_name.u.cfunc();
            }
            else if (e_name.type == ET_EXECUTABLE_ARRAY) {
                // 実行可能配列。バイトコードを実行
                eval_exec_array(e_name.u.byte_codes);
            }
            else {
                // ユーザ定義の変数。変数が指す値を stack に push する
                stack_push(&e_name);
            }
            break;
        }
        case ET_C_FUNC:
            e.u.cfunc();
            break;
        case ET_EXECUTABLE_ARRAY:
            stack_push(&e);
            break;
        }
    }
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
                else if (e.type == ET_EXECUTABLE_ARRAY) {
                    // 実行可能配列。バイトコードを実行
                    eval_exec_array(e.u.byte_codes);
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
            case LT_OPEN_CURLY: {
                StackElement e;
                compile_exec_array(&e);
                stack_push(&e);
                break;
            }
            case LT_SPACE:
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

static void test_eval_num_eq_true() {
    char* input = "3 3 eq";
    int expects[1] = { 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_eq_false() {
    char* input = "3 2 eq";
    int expects[1] = { 0 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_neq_true() {
    char* input = "3 2 neq";
    int expects[1] = { 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_neq_false() {
    char* input = "3 3 neq";
    int expects[1] = { 0 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_gt_true_when_greater() {
    char* input = "3 2 gt";
    int expects[1] = { 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_gt_false_when_equal() {
    char* input = "3 3 gt";
    int expects[1] = { 0 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_gt_false_when_less() {
    char* input = "3 4 gt";
    int expects[1] = { 0 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_ge_true_when_greater() {
    char* input = "3 2 ge";
    int expects[1] = { 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_ge_true_when_equal() {
    char* input = "3 3 ge";
    int expects[1] = { 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_ge_false_when_less() {
    char* input = "3 4 ge";
    int expects[1] = { 0 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_lt_false_when_greater() {
    char* input = "3 2 lt";
    int expects[1] = { 0 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_lt_false_when_equal() {
    char* input = "3 3 lt";
    int expects[1] = { 0 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_lt_true_when_less() {
    char* input = "3 4 lt";
    int expects[1] = { 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_le_false_when_greater() {
    char* input = "3 2 le";
    int expects[1] = { 0 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_le_true_when_equal() {
    char* input = "3 3 le";
    int expects[1] = { 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_le_true_when_less() {
    char* input = "3 4 le";
    int expects[1] = { 1 };

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

static void test_eval_num_pop() {
    char* input = "3 4 pop";
    int expects[1] = { 3 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_num_exch() {
    char* input = "3 4 5 exch";
    int expects[2] = { 4, 5 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 2);
}

static void test_eval_num_dup() {
    char* input = "3 4 dup";
    int expects[3] = { 4, 4, 3 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 3);
}

static void test_eval_exec_array_num() {
    char* input = "/num { 42 } def num";
    int expects[1] = { 42 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_exec_array_num_many() {
    char* input = "/num { 42 7 3 } def num";
    int expects[3] = { 3, 7, 42 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 3);
}

static void test_eval_exec_array_func() {
    char* input = "/add3 { 3 add } def 4 add3";
    int expects[1] = { 7 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_exec_array_num_nested() {
    char* input = "/zz { 6 } def /yy { 4 zz 5 } def /xx { 1 2 yy 3 } def xx";
    int expects[6] = { 3, 5, 6, 4, 2, 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 6);
}

static void test_eval_exec_array_func_nested() {
    char* input = "/double7 { /double { 2 mul } def 7 double } def double7";
    int expects[1] = { 14 };

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

static void test_dict_overwritten_not_head_name_found() {
    int ninput = 3;
    // ハッシュテーブルの head 以外の要素を上書きする場合
    char* input_keys[] = { "oof", "foo", "foo" };
    StackElement input_elements[] = { { ET_NUMBER, {.number = 42} }, { ET_NUMBER, {.number = 420} }, { ET_NUMBER, {.number = 4200} } };

    char* target_key = "foo";

    int expect_found = 1;
    int expect_value = 4200;

    verify_dict_put_and_get(ninput, input_keys, input_elements, target_key, expect_found, expect_value);
}

int main() {
    register_primitives();

    test_eval_name_one();
    test_eval_num_one();
    test_eval_num_two();
    test_eval_num_add();
    test_eval_num_add_many();
    test_eval_num_sub();
    test_eval_num_mul();
    test_eval_num_div();
    test_eval_num_eq_true();
    test_eval_num_eq_false();
    test_eval_num_neq_true();
    test_eval_num_neq_false();
    test_eval_num_gt_true_when_greater();
    test_eval_num_gt_false_when_equal();
    test_eval_num_gt_false_when_less();
    test_eval_num_ge_true_when_greater();
    test_eval_num_ge_true_when_equal();
    test_eval_num_ge_false_when_less();
    test_eval_num_lt_false_when_greater();
    test_eval_num_lt_false_when_equal();
    test_eval_num_lt_true_when_less();
    test_eval_num_le_false_when_greater();
    test_eval_num_le_true_when_equal();
    test_eval_num_le_true_when_less();

    test_eval_num_pop();
    test_eval_num_exch();
    test_eval_num_dup();

    test_eval_num_def();

    test_eval_exec_array_num();
    test_eval_exec_array_num_many();
    test_eval_exec_array_func();
    test_eval_exec_array_num_nested();
    test_eval_exec_array_func_nested();

    test_dict_no_element_name_not_found();
    test_dict_name_found();
    test_dict_name_found_when_hash_collides();
    test_dict_name_not_found();
    test_dict_overwritten_name_found();
    test_dict_overwritten_not_head_name_found();

    return 0;
}