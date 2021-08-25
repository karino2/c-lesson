#include "clesson.h"
#include "stack.h"
#include "parser.h"
#include "dict.h"
#include "continuation.h"
#include <assert.h>

#define MAX_NAME_OP_NUMBERS 256

static void eval_exec_array(StackElementArray* exec_array);

static int c_add(int n, int m) { return n + m; }
static int c_sub(int n, int m) { return n - m; }
static int c_mul(int n, int m) { return n * m; }
static int c_div(int n, int m) { return n / m; }
static int c_mod(int n, int m) { return n % m; }
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
static void mod_op() { calc_op("mod", c_mod); }
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

static void index_op() {
    StackElement ne;
    stack_pop(&ne);
    if (ne.type != ET_NUMBER) {
        printf("index expects number, but got %d\n", ne.type);
        exit(1);
    }

    int depth = ne.u.number;

    StackElement target;
    stack_peek(depth, &target);
    stack_push(&target);
}

static void roll_op() {
    StackElement nre, nee;
    stack_pop(&nre);
    stack_pop(&nee);
    if (nre.type != ET_NUMBER || nee.type != ET_NUMBER) {
        printf("roll expects number, but got (%d, %d)\n", nre.type, nee.type);
        exit(1);
    }

    int nelems = nee.u.number;
    int nrolls = nre.u.number % nelems;

    StackElement* rolled_es = malloc(sizeof(StackElement) * nelems);
    for (int i = 0; i < nelems; i++) {
        stack_pop(&rolled_es[(i + nelems - nrolls) % nelems]);
    }
    for (int i = nelems - 1; i >= 0; i--) {
        stack_push(&rolled_es[i]);
    }
}

static void exec_op() {
    StackElement e;
    stack_pop(&e);
    if (e.type != ET_EXECUTABLE_ARRAY) {
        printf("exec expects executable array, but got %d\n", e.type);
        exit(1);
    }

    eval_exec_array(e.u.byte_codes);
}

static void if_op() {
    StackElement p, b;
    stack_pop(&p);
    stack_pop(&b);
    if (p.type != ET_EXECUTABLE_ARRAY || b.type != ET_NUMBER) {
        printf("if expects executable array and number, but got (%d, %d)\n", p.type, b.type);
        exit(1);
    }

    if (b.u.number) {
        eval_exec_array(p.u.byte_codes);
    }
}

static void ifelse_op() {
    StackElement pf, pt, b;
    stack_pop(&pf);
    stack_pop(&pt);
    stack_pop(&b);
    if (pf.type != ET_EXECUTABLE_ARRAY || pt.type != ET_EXECUTABLE_ARRAY || b.type != ET_NUMBER) {
        printf("ifelse expects two executable arrays and number, but got (%d, %d, %d)\n", pf.type, pt.type, b.type);
        exit(1);
    }

    if (b.u.number) {
        eval_exec_array(pt.u.byte_codes);
    }
    else {
        eval_exec_array(pf.u.byte_codes);
    }
}

static void while_op() {
    StackElement p, c;
    stack_pop(&p);
    stack_pop(&c);
    if (p.type != ET_EXECUTABLE_ARRAY || c.type != ET_EXECUTABLE_ARRAY) {
        printf("while expects two executable arrays, but got (%d, %d)\n", p.type, c.type);
        exit(1);
    }

    while (1) {
        eval_exec_array(c.u.byte_codes);
        StackElement b;
        stack_pop(&b);
        if (b.type != ET_NUMBER) {
            printf("while expects number as a result of condition execution, but got %d\n", b.type);
            exit(1);
        }

        if (b.u.number) {
            eval_exec_array(p.u.byte_codes);
        }
        else {
            break;
        }
    }
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

void register_primitives() {
    register_primitive("add", add_op);
    register_primitive("sub", sub_op);
    register_primitive("mul", mul_op);
    register_primitive("div", div_op);
    register_primitive("mod", mod_op);
    register_primitive("eq", eq_op);
    register_primitive("neq", neq_op);
    register_primitive("gt", gt_op);
    register_primitive("ge", ge_op);
    register_primitive("lt", lt_op);
    register_primitive("le", le_op);

    register_primitive("pop", pop_op);
    register_primitive("exch", exch_op);
    register_primitive("dup", dup_op);
    register_primitive("index", index_op);
    register_primitive("roll", roll_op);

    register_primitive("exec", exec_op);
    register_primitive("if", if_op);
    register_primitive("ifelse", ifelse_op);
    register_primitive("while", while_op);

    register_primitive("def", def_op);
}

typedef struct {
    StackElement elems[MAX_NAME_OP_NUMBERS];
    int pos;
} Emitter;

static void emit(Emitter* em, StackElement elem) {
    em->elems[em->pos++] = elem;
}

static int compile_exec_array(int prev_ch, StackElement* out_element) {
    int ch = prev_ch;
    Token token = {
        LT_UNKNOWN,
        {0}
    };

    int i = 0;
    Emitter em;
    em.pos = 0;

    do {
        ch = parse_one(ch, &token);

        if (token.ltype != LT_UNKNOWN) {
            switch (token.ltype) {
            case LT_NUMBER:
                emit(&em, number_element(token.u.number));
                break;
            case LT_EXECUTABLE_NAME:
                if (streq(token.u.name, "ifelse")) {
                    emit(&em, number_element(3));
                    emit(&em, number_element(2));
                    emit(&em, ename_element("roll"));
                    emit(&em, number_element(5));
                    emit(&em, ename_element("jmp_not_if"));
                    emit(&em, ename_element("pop"));
                    emit(&em, ename_element("exec"));
                    emit(&em, number_element(4));
                    emit(&em, ename_element("jmp"));
                    emit(&em, ename_element("exch"));
                    emit(&em, ename_element("pop"));
                    emit(&em, ename_element("exec"));
                }
                else {
                    emit(&em, ename_element(token.u.name));
                }
                break;
            case LT_LITERAL_NAME:
                emit(&em, lname_element(token.u.name));
                break;
            case LT_OPEN_CURLY: {
                StackElement e;
                ch = compile_exec_array(ch, &e);
                emit(&em, e);
                break;
            }
            case LT_CLOSE_CURLY:
            case LT_SPACE:
                break;
            default:
                break;
            }
        }
    } while (token.ltype != LT_END_OF_FILE && token.ltype != LT_CLOSE_CURLY);

    StackElementArray* array = malloc(sizeof(StackElementArray) + sizeof(StackElement) * em.pos);
    array->len = em.pos;
    memcpy(array->elements, em.elems, sizeof(StackElement) * em.pos);
    *out_element = earray_element(array);

    return ch;
}

static void eval_continuation(Continuation* cont) {
    for (int pc = cont->pc; pc < cont->exec_array->len; pc++) {
        StackElement elem = cont->exec_array->elements[pc];
        switch (elem.type) {
        case ET_NUMBER:
        case ET_LITERAL_NAME:
            stack_push(&elem);
            break;
        case ET_EXECUTABLE_NAME: {
            if (streq(elem.u.name, "exec")) {
                StackElement exec_array;
                stack_pop(&exec_array);
                if (exec_array.type != ET_EXECUTABLE_ARRAY) {
                    printf("exec expects executable array, but got: %d\n", exec_array.type);
                    exit(1);
                }
                co_stack_push(&(Continuation) { cont->exec_array, pc + 1 });
                co_stack_push(&(Continuation) { exec_array.u.byte_codes, 0 });
                return;
            }
            else if (streq(elem.u.name, "if")) {
                StackElement exec_array, cond;
                stack_pop(&exec_array);
                stack_pop(&cond);
                if (exec_array.type != ET_EXECUTABLE_ARRAY || cond.type != ET_NUMBER) {
                    printf("if expects executable array and number, but got (%d, %d)\n", exec_array.type, cond.type);
                    exit(1);
                }

                co_stack_push(&(Continuation) { cont->exec_array, pc + 1 });
                if (cond.u.number) {
                    co_stack_push(&(Continuation) { exec_array.u.byte_codes, 0 });
                }
                return;
            }
            else if (streq(elem.u.name, "jmp")) {
                StackElement num;
                stack_pop(&num);
                if (num.type != ET_NUMBER) {
                    printf("jmp expects number, but got %d\n", num.type);
                    exit(1);
                }

                co_stack_push(&(Continuation) { cont->exec_array, pc + num.u.number });
                return;
            }
            else if (streq(elem.u.name, "jmp_not_if")) {
                StackElement num, cond;
                stack_pop(&num);
                stack_pop(&cond);
                if (num.type != ET_NUMBER || cond.type != ET_NUMBER) {
                    printf("jmp_not_if expects numbers, but got (%d, %d)\n", num.type, cond.type);
                    exit(1);
                }

                if (!cond.u.number) {
                    co_stack_push(&(Continuation) { cont->exec_array, pc + num.u.number });
                }
                else {
                    co_stack_push(&(Continuation) { cont->exec_array, pc + 1 });
                }
                return;
            }

            StackElement exec_name;
            if (!dict_get(elem.u.name, &exec_name)) {
                printf("fail to resolve name %s\n", elem.u.name);
                exit(1);
            }

            if (exec_name.type == ET_C_FUNC) {
                // プリミティブ。事前に登録してある関数を実行
                exec_name.u.cfunc();
            }
            else if (exec_name.type == ET_EXECUTABLE_ARRAY) {
                // 実行可能配列。実行中の continuation と次に実行したい continuation を順番に co_stack に積む
                co_stack_push(&(Continuation) { cont->exec_array, pc + 1 });
                co_stack_push(&(Continuation) { exec_name.u.byte_codes, 0 });
                return;
            }
            else {
                // ユーザ定義の変数。変数が指す値を stack に push する
                stack_push(&exec_name);
            }
            break;
        }
        case ET_C_FUNC:
            elem.u.cfunc();
            break;
        case ET_EXECUTABLE_ARRAY:
            stack_push(&elem);
            break;
        }
    }
}

static void eval_exec_array(StackElementArray* exec_array) {
    co_stack_push(&(Continuation) { exec_array, 0 });

    Continuation cont;
    while (co_stack_pop(&cont)) {
        eval_continuation(&cont);
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
                ch = compile_exec_array(ch, &e);
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

void exec_tests() {
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