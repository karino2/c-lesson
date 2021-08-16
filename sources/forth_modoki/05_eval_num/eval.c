#include "clesson.h"
#include "stack.h"
#include <assert.h>

void eval() {}

static void verify_stack_pop_number_eq(int expects[], int nexpects) {
    for (int i = 0; i < nexpects; i++) {
        StackElement* actual = stack_pop();

        assert(actual != NULL);
        assert(actual->type == ET_NUMBER);
        assert(actual->u.number == expects[i]);
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


int main() {
    test_eval_num_one();
    test_eval_num_two();
    test_eval_num_add();

    return 0;
}
