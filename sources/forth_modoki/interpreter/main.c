#include "cl.h"
#include "compile.h"
#include "eval.h"
#include "stack.h"
#include "dict.h"
#include <stdio.h>
#include <assert.h>

static void tests();

int main(int argc, char* argv[]) {
    switch (argc) {
    case 1:
        tests();
        return 0;
    case 2: {
        FILE* f = fopen(argv[1], "r");
        if (!f) {
            printf("failed to open file\n");
            exit(1);
        }

        cl_getc_set_src_file(f);

        register_compile_primitives();
        register_primitives();
        eval();

        fclose(f);

        stack_print_all();
        return 0;
    }
    default:
        printf("number of args should be 0 or 1\n");
        return 1;
    }
}

static void verify_stack_pop_number_eq(int expects[], int nexpects) {
    for (int i = 0; i < nexpects; i++) {
        Element actual;
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

static void test_eval_num_two_separated_by_newline() {
    char* input = "123\n456";
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

static void test_eval_num_mod() {
    char* input = "27 5 mod";
    int expects[1] = { 2 };

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

static void test_eval_num_index() {
    char* input = "2 4 6 8 2 index";
    int expects[5] = { 4, 8, 6, 4, 2 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 5);
}

static void test_eval_num_index_top() {
    char* input = "2 4 6 8 0 index";
    int expects[5] = { 8, 8, 6, 4, 2 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 5);
}

static void test_eval_num_roll() {
    char* input = "1 2 3 4 5 6 7 4 3 roll";
    int expects[7] = { 4, 7, 6, 5, 3, 2, 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 7);
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

static void test_eval_exec_array_exec_nested() {
    char* input = "/f { {1 3 add} exec 3} def f";
    int expects[2] = { 3, 4 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 2);
}

static void test_eval_exec_nums() {
    char* input = "{ 1 2 3 } exec";
    int expects[3] = { 3, 2, 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 3);
}

static void test_eval_exec_func() {
    char* input = "{ 1 2 add } exec";
    int expects[1] = { 3 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_exec_func_in_executable_array() {
    char* input = "{ { 1 2 add } exec } exec";
    int expects[1] = { 3 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_if_true() {
    char* input = "3 1 { 1 add } if";
    int expects[1] = { 4 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_if_false() {
    char* input = "3 0 { 1 add } if";
    int expects[1] = { 3 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_ifelse_true() {
    char* input = "3 1 { 1 add } { 2 add } ifelse";
    int expects[1] = { 4 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_ifelse_false() {
    char* input = "3 0 { 1 add } { 2 add } ifelse";
    int expects[1] = { 5 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_ifelse_true_in_executable_array() {
    char* input = "{ 3 1 { 1 add } { 2 add } ifelse } exec";
    int expects[1] = { 4 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_ifelse_false_in_executable_array() {
    char* input = "{ 3 0 { 1 add } { 2 add } ifelse } exec";
    int expects[1] = { 5 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_ifelse_then_num() {
    char* input = "3 1 { 1 add } { 2 add } ifelse 7";
    int expects[2] = { 7, 4 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 2);
}

static void test_eval_ifelse_incomplete_executable_array() {
    char* input = "/a { {345} ifelse} def 1 {123} a";
    int expects[1] = { 123 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_while() {
    char* input = "0 { dup 3 lt } { dup 1 add } while";
    int expects[4] = { 3, 2, 1, 0 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 4);
}

static void test_eval_while_in_executable_array() {
    char* input = "{ 0 { dup 3 lt } { dup 1 add } while } exec";
    int expects[4] = { 3, 2, 1, 0 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 4);
}

static void test_eval_while_then_num() {
    char* input = "0 { dup 3 lt } { dup 1 add } while 7";
    int expects[5] = { 7, 3, 2, 1, 0 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 5);
}

static void test_eval_comments() {
    char* input = "1 \% hello this is comment\n2\n3 \% ignore me";
    int expects[3] = { 3, 2, 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 3);
}

static void test_eval_factorial() {
    char* input = "/factorial { dup {dup 1 gt} { 1 sub exch 1 index mul exch } while pop } def 10 factorial";

    int expects[1] = { 3628800 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_nested_exec() {
    char* input = "/c { 3 } def /b { { c } exec 2 } def /a { b 1 } def a";

    int expects[3] = { 1, 2, 3 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 3);
}

static void test_eval_nested_if_true() {
    char* input = "/c { 3 } def /b { 1 { c } if 2 } def /a { b 1 } def a";

    int expects[3] = { 1, 2, 3 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 3);
}

static void test_eval_nested_if_false() {
    char* input = "/c { 3 } def /b { 0 { c } if 2 } def /a { b 1 } def a";

    int expects[2] = { 1, 2 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 2);
}

static void test_eval_nested_ifelse_true() {
    char* input = "/d { 4 } def /c { 3 } def /b { 1 { c } { d } ifelse 2 } def /a { b 1 } def a";

    int expects[3] = { 1, 2, 3 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 3);
}

static void test_eval_nested_ifelse_false() {
    char* input = "/d { 4 } def /c { 3 } def /b { 0 { c } { d } ifelse 2 } def /a { b 1 } def a";

    int expects[3] = { 1, 2, 4 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 3);
}

static void test_eval_jmp_positive() {
    char* input = "{ 1 2 jmp 3 4 5 } exec";

    int expects[3] = { 5, 4, 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 3);
}

static void test_eval_jmp_positive_over() {
    char* input = "{ 1 4 jmp 3 4 5 } exec";

    int expects[1] = { 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void test_eval_jmp_not_if_not_executed() {
    char* input = "{ 1 1 2 jmp_not_if 3 4 5 } exec";

    int expects[4] = { 5, 4, 3, 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 4);
}

static void test_eval_jmp_not_if_positive() {
    char* input = "{ 1 0 2 jmp_not_if 3 4 5 } exec";

    int expects[3] = { 5, 4, 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 3);
}

static void test_eval_jmp_not_if_positive_over() {
    char* input = "{ 1 0 4 jmp_not_if 3 4 5 } exec";

    int expects[1] = { 1 };

    cl_getc_set_src(input);

    eval();

    verify_stack_pop_number_eq(expects, 1);
}

static void verify_dict_put_and_get(
    int ninput, char* input_keys[], Element input_elements[],
    char* target_key, int expect_found, int expect_value
) {
    for (int i = 0; i < ninput; i++) {
        eval_dict_put(input_keys[i], &input_elements[i]);
    }

    Element actual_value;
    int actual_found = eval_dict_get(target_key, &actual_value);

    assert(actual_found == expect_found);
    if (expect_found) {
        assert(actual_value.type == ET_NUMBER);
        assert(actual_value.u.number == expect_value);
    }

    // ユーザが put した element を掃除するために、一度全ての element を削除してからプリミティブを登録し直す
    eval_dict_clear();
    register_primitives();
}

static void test_dict_no_element_name_not_found() {
    int ninput = 0;
    char* input_keys[] = {};
    Element input_elements[] = {};

    char* target_key = "foo";

    int expect_found = 0;
    int expect_value = -1;

    verify_dict_put_and_get(ninput, input_keys, input_elements, target_key, expect_found, expect_value);
}

static void test_dict_name_found() {
    int ninput = 1;
    char* input_keys[] = { "foo" };
    Element input_elements[] = { { ET_NUMBER, {.number = 42} } };

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
    Element input_elements[] = { { ET_NUMBER, {.number = 42} }, { ET_NUMBER, {.number = 24} } };

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
    Element input_elements[] = { { ET_NUMBER, {.number = 42} } };

    char* target_key = "bar";

    int expect_found = 0;
    int expect_value = -1;

    verify_dict_put_and_get(ninput, input_keys, input_elements, target_key, expect_found, expect_value);
}

static void test_dict_overwritten_name_found() {
    int ninput = 2;
    char* input_keys[] = { "foo", "foo" };
    Element input_elements[] = { { ET_NUMBER, {.number = 42} }, { ET_NUMBER, {.number = 420} } };

    char* target_key = "foo";

    int expect_found = 1;
    int expect_value = 420;

    verify_dict_put_and_get(ninput, input_keys, input_elements, target_key, expect_found, expect_value);
}

static void test_dict_overwritten_not_head_name_found() {
    int ninput = 3;
    // ハッシュテーブルの head 以外の要素を上書きする場合
    char* input_keys[] = { "oof", "foo", "foo" };
    Element input_elements[] = { { ET_NUMBER, {.number = 42} }, { ET_NUMBER, {.number = 420} }, { ET_NUMBER, {.number = 4200} } };

    char* target_key = "foo";

    int expect_found = 1;
    int expect_value = 4200;

    verify_dict_put_and_get(ninput, input_keys, input_elements, target_key, expect_found, expect_value);
}

static void tests() {
    register_compile_primitives();
    register_primitives();

    test_eval_num_one();
    test_eval_num_two();
    test_eval_num_two_separated_by_newline();
    test_eval_num_add();
    test_eval_num_add_many();
    test_eval_num_sub();
    test_eval_num_mul();
    test_eval_num_div();
    test_eval_num_mod();
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
    test_eval_num_index();
    test_eval_num_index_top();
    test_eval_num_roll();

    test_eval_num_def();

    test_eval_exec_array_num();
    test_eval_exec_array_num_many();
    test_eval_exec_array_func();
    test_eval_exec_array_num_nested();
    test_eval_exec_array_func_nested();
    test_eval_exec_array_exec_nested();

    test_eval_exec_nums();
    test_eval_exec_func();
    test_eval_exec_func_in_executable_array();
    test_eval_if_true();
    test_eval_if_false();
    test_eval_ifelse_true();
    test_eval_ifelse_false();
    test_eval_ifelse_true_in_executable_array();
    test_eval_ifelse_false_in_executable_array();
    test_eval_ifelse_then_num();
    test_eval_ifelse_incomplete_executable_array();
    test_eval_while();
    test_eval_while_in_executable_array();
    test_eval_while_then_num();

    test_eval_comments();

    test_eval_factorial();

    test_eval_nested_exec();
    test_eval_nested_if_true();
    test_eval_nested_if_false();
    test_eval_nested_ifelse_true();
    test_eval_nested_ifelse_false();

    test_eval_jmp_positive();
    test_eval_jmp_positive_over();
    test_eval_jmp_not_if_positive();
    test_eval_jmp_not_if_positive_over();
    test_eval_jmp_not_if_not_executed();

    test_dict_no_element_name_not_found();
    test_dict_name_found();
    test_dict_name_found_when_hash_collides();
    test_dict_name_not_found();
    test_dict_overwritten_name_found();
    test_dict_overwritten_not_head_name_found();
}