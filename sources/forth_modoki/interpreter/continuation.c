#include "continuation.h"

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