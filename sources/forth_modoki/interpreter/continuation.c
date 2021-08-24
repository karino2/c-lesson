#include "continuation.h"
#include <stdio.h>

#define CO_STACK_SIZE 1024

static Continuation co_stack[CO_STACK_SIZE];

static int top = -1;


void co_stack_push(Continuation* continuation) {
    co_stack[++top] = *continuation;
}

int co_stack_pop(Continuation* out_continuation) {
    if (top < 0) { return 0; }
    *out_continuation = co_stack[top--];
    return 1;
}

void co_stack_print_all() {
    printf("-- costack --\n");
    for (int i = top; i >= 0; i--) {
        Continuation c = co_stack[i];
        char fbuf[1024], cbuf[1024];

        stack_element_debug_string(fbuf, &c.exec_array->elements[0]);
        stack_element_debug_string(cbuf, &c.exec_array->elements[c.pc]);

        printf("%d: current(pc=%d/len=%d) [%s] first [%s] \n", i, c.pc, c.exec_array->len, cbuf, fbuf);
    }
    printf("-------------\n");
}
