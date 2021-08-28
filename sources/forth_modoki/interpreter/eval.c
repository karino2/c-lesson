#include "eval.h"
#include "cl.h"
#include "stack.h"
#include "parser.h"
#include "dict.h"
#include "continuation.h"
#include "compile.h"
#include <assert.h>

static void eval_exec_array(ElementArray* exec_array);

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
    Element e1, e2;
    stack_pop(&e1);
    stack_pop(&e2);
    if (e1.type != ET_NUMBER || e2.type != ET_NUMBER) {
        printf("%s expects number operands, but got (%d, %d)\n", name, e1.type, e2.type);
        exit(1);
    }

    Element element = { ET_NUMBER, {.number = c_op(e2.u.number, e1.u.number)} };
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
    Element e;
    stack_pop(&e);
}

static void exch_op() {
    Element e1, e2;
    stack_pop(&e1);
    stack_pop(&e2);
    stack_push(&e1);
    stack_push(&e2);
}

static void dup_op() {
    Element e;
    stack_pop(&e);
    stack_push(&e);
    stack_push(&e);
}

static void index_op() {
    Element ne;
    stack_pop(&ne);
    if (ne.type != ET_NUMBER) {
        printf("index expects number, but got %d\n", ne.type);
        exit(1);
    }

    int depth = ne.u.number;

    Element target;
    stack_peek(depth, &target);
    stack_push(&target);
}

static void roll_op() {
    Element nre, nee;
    stack_pop(&nre);
    stack_pop(&nee);
    if (nre.type != ET_NUMBER || nee.type != ET_NUMBER) {
        printf("roll expects number, but got (%d, %d)\n", nre.type, nee.type);
        exit(1);
    }

    int nelems = nee.u.number;
    int nrolls = nre.u.number % nelems;

    Element* rolled_es = malloc(sizeof(Element) * nelems);
    for (int i = 0; i < nelems; i++) {
        stack_pop(&rolled_es[(i + nelems - nrolls) % nelems]);
    }
    for (int i = nelems - 1; i >= 0; i--) {
        stack_push(&rolled_es[i]);
    }
}

static void exec_op() {
    Element e;
    stack_pop(&e);
    if (e.type != ET_EXECUTABLE_ARRAY) {
        printf("exec expects executable array, but got %d\n", e.type);
        exit(1);
    }

    eval_exec_array(e.u.byte_codes);
}

static void if_op() {
    Element p, b;
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
    Element pf, pt, b;
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
    Element p, c;
    stack_pop(&p);
    stack_pop(&c);
    if (p.type != ET_EXECUTABLE_ARRAY || c.type != ET_EXECUTABLE_ARRAY) {
        printf("while expects two executable arrays, but got (%d, %d)\n", p.type, c.type);
        exit(1);
    }

    while (1) {
        eval_exec_array(c.u.byte_codes);
        Element b;
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

static void repeat_op() {
    Element p, n;
    stack_pop(&p);
    stack_pop(&n);
    if (p.type != ET_EXECUTABLE_ARRAY || n.type != ET_NUMBER) {
        printf("repeat expects executable array and number, but got (%d, %d)\n", p.type, n.type);
        exit(1);
    }

    for (int i = 0; i < n.u.number; i++) {
        eval_exec_array(p.u.byte_codes);
    }
}

static void def_op() {
    Element value, key;
    stack_pop(&value);
    stack_pop(&key);
    if ((value.type != ET_NUMBER && value.type != ET_EXECUTABLE_ARRAY) || key.type != ET_LITERAL_NAME) {
        printf("def expects number or executable array as value and literal name as key, but got (%d, %d)\n", value.type, key.type);
        exit(1);
    }

    eval_dict_put(key.u.name, &value);
}

static void register_primitive(char* key, void (*op)()) {
    eval_dict_put(key, &(Element){ ET_C_FUNC, { .cfunc = op } });
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
    register_primitive("repeat", repeat_op);

    register_primitive("def", def_op);
}

static void eval_continuation(Continuation* cont) {
    for (int pc = cont->pc; pc < cont->exec_array->len; pc++) {
        Element elem = cont->exec_array->elements[pc];
        switch (elem.type) {
        case ET_NUMBER:
        case ET_LITERAL_NAME:
            stack_push(&elem);
            break;
        case ET_EXECUTABLE_NAME: {
            Element exec_name;
            if (!eval_dict_get(elem.u.name, &exec_name)) {
                printf("fail to resolve name %s\n", elem.u.name);
                exit(1);
            }

            if (exec_name.type == ET_C_FUNC) {
                // プリミティブ。事前に登録してある関数を実行
                exec_name.u.cfunc();
            }
            else if (exec_name.type == ET_EXECUTABLE_ARRAY) {
                // 実行可能配列。実行中の continuation と次に実行したい continuation を順番に co_stack に積む
                ContinuationElement cur_cont = continuation(cont->exec_array, pc + 1);
                ContinuationElement next_cont = continuation(exec_name.u.byte_codes, 0);
                co_stack_push(&cur_cont);
                co_stack_push(&next_cont);
                return;
            }
            else {
                // ユーザ定義の変数。変数が指す値を stack に push する
                stack_push(&exec_name);
            }
            break;
        }
        case ET_C_FUNC:
            printf("c func should be converted in compile time, but exists in eval\n");
            exit(1);
        case ET_COMPILE_FUNC:
            printf("compile func should be converted in compile time, but exists in eval\n");
            exit(1);
        case ET_PRIMITIVE:
            switch (elem.u.op) {
            case OP_EXEC: {
                Element exec_array;
                stack_pop(&exec_array);
                if (exec_array.type != ET_EXECUTABLE_ARRAY) {
                    printf("exec expects executable array, but got: %d\n", exec_array.type);
                    exit(1);
                }
                ContinuationElement cur_cont = continuation(cont->exec_array, pc + 1);
                ContinuationElement next_cont = continuation(exec_array.u.byte_codes, 0);
                co_stack_push(&cur_cont);
                co_stack_push(&next_cont);
                return;
            }
            case OP_JMP: {
                Element num;
                stack_pop(&num);
                if (num.type != ET_NUMBER) {
                    printf("jmp expects number, but got %d\n", num.type);
                    exit(1);
                }

                ContinuationElement c = continuation(cont->exec_array, pc + num.u.number);
                co_stack_push(&c);
                return;
            }
            case OP_JMP_NOT_IF: {
                Element num, cond;
                stack_pop(&num);
                stack_pop(&cond);
                if (num.type != ET_NUMBER || cond.type != ET_NUMBER) {
                    printf("jmp_not_if expects numbers, but got (%d, %d)\n", num.type, cond.type);
                    exit(1);
                }

                if (!cond.u.number) {
                    ContinuationElement c = continuation(cont->exec_array, pc + num.u.number);
                    co_stack_push(&c);
                }
                else {
                    ContinuationElement c = continuation(cont->exec_array, pc + 1);
                    co_stack_push(&c);
                }
                return;
            }
            case OP_STORE: {
                Element var;
                stack_pop(&var);

                ContinuationElement v = local_var(&var);
                co_stack_push(&v);

                ContinuationElement c = continuation(cont->exec_array, pc + 1);
                co_stack_push(&c);
                return;
            }
            case OP_LOAD: {
                Element num;
                stack_pop(&num);
                if (num.type != ET_NUMBER) {
                    printf("load expects number, but got %d\n", num.type);
                    exit(1);
                }

                ContinuationElement var;
                if (!co_stack_peek(num.u.number, &var)) {
                    printf("failed to load local var\n");
                    exit(1);
                }
                stack_push(var.u.elem);

                ContinuationElement c = continuation(cont->exec_array, pc + 1);
                co_stack_push(&c);
                return;
            }
            case OP_LPOP: {
                ContinuationElement var;
                if (!co_stack_pop(&var)) {
                    printf("failed to pop local var\n");
                    exit(1);
                }

                if (var.type != CE_LOCAL_VAR) {
                    printf("popped element is expected be local var, but got %d\n", var.type);
                    exit(1);
                }

                stack_push(var.u.elem);

                ContinuationElement c = continuation(cont->exec_array, pc + 1);
                co_stack_push(&c);
                return;
            }
            }
            break;
        case ET_EXECUTABLE_ARRAY:
            stack_push(&elem);
            break;
        }
    }
}

static void eval_exec_array(ElementArray* exec_array) {
    ContinuationElement c = continuation(exec_array, 0);
    co_stack_push(&c);

    ContinuationElement elem;
    while (co_stack_pop_current_continuation(&elem)) {
        eval_continuation(elem.u.cont);
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
                Element element = { ET_NUMBER, {.number = token.u.number} };
                stack_push(&element);
                break;
            }
            case LT_EXECUTABLE_NAME: {
                Element e;
                if (!eval_dict_get(token.u.name, &e)) {
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
                Element element = { ET_LITERAL_NAME, {.name = token.u.name} };
                stack_push(&element);
                break;
            }
            case LT_OPEN_CURLY: {
                Element e;
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