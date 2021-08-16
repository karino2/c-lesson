#include "clesson.h"
#include "stack.h"
#include <assert.h>

void eval() {}

static void test_eval_num_one() {
    char* input = "123";
    int expect = 123;

    cl_getc_set_src(input);

    eval();

    StackElement* popped = stack_pop();

    assert(popped != NULL);
    assert(popped->type == ET_NUMBER);

    int actual = popped->u.number;

    assert(expect == actual);

}

static void test_eval_num_two() {
    char* input = "123 456";
    int expect1 = 456;
    int expect2 = 123;

    cl_getc_set_src(input);

    eval();

    StackElement* popped1 = stack_pop();
    StackElement* popped2 = stack_pop();

    assert(popped1 != NULL);
    assert(popped1->type == ET_NUMBER);

    assert(popped2 != NULL);
    assert(popped2->type == ET_NUMBER);

    int actual1 = popped1->u.number;
    int actual2 = popped2->u.number;

    assert(expect1 == actual1);
    assert(expect2 == actual2);
}


static void test_eval_num_add() {
    char* input = "1 2 add";
    int expect = 3;

    cl_getc_set_src(input);

    eval();

    StackElement* popped = stack_pop();

    assert(popped != NULL);
    assert(popped->type == ET_NUMBER);

    int actual = popped->u.number;

    assert(expect == actual);
}


int main() {
    test_eval_num_one();
    test_eval_num_two();
    test_eval_num_add();

    return 0;
}
