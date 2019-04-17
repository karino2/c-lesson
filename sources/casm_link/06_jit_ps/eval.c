#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static int stack_pos = 0;
static int stack[1024];
void stack_push(int val) {
    stack[stack_pos++] = val;
}
int stack_pop() {
    if(stack_pos == 0) {
        fprintf(stderr, "stack pop while stack is empty, exit.\n");
        exit(1);
    }
    return stack[--stack_pos];
}

int eval(int r0, int r1, char *str) {    
    struct Substr remain={str, strlen(str)};
    int val;

    stack_pos = 0;

    while(!is_end(&remain)) {
        skip_space(&remain);
        if(is_number(remain.ptr)) {
            stack_push(parse_number(remain.ptr));
            skip_token(&remain);
            continue;
        }else if(is_register(remain.ptr)) {
            if(remain.ptr[1] == '1') {
                val = r1;
            } else {
                val = r0;
            }
            stack_push(val);
            skip_token(&remain);
            continue;
        } else {
            // must be op.
            int arg1, arg2;

            val = parse_word(&remain);
            skip_token(&remain);

            arg2 = stack_pop();
            arg1 = stack_pop();

            switch(val) {
                case OP_ADD:
                    stack_push(arg1+arg2);
                    break;
                case OP_SUB:
                    stack_push(arg1-arg2);
                    break;
                case OP_MUL:
                    stack_push(arg1*arg2);                
                    break;
                case OP_DIV:
                    stack_push(arg1/arg2);
                    break;
            }
            continue;
        }
    }
    return stack_pop();
}

#include "test_util.h"

static void test_eval() {
    int actual;
    actual = eval(1, 5, "3 7 add r1 sub 4 mul r0 add");
    assert_int_eq(21, actual);    
}

static void run_unit_tests() {
    test_eval();

    printf("all test done\n");
}

#if 0
int main() {
    run_unit_tests();
    return 0;
}
#endif
