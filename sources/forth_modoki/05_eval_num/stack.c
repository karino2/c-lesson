#include "stack.h"

#define STACK_SIZE 1024

static StackElement stack[STACK_SIZE];

static int top = -1;

static int is_stack_empty() {
    return top < 0;
}

void stack_push(StackElement literal) {
    stack[++top] = literal;
}

StackElement* stack_pop() {
    if (is_stack_empty()) return NULL;
    return &stack[top--];
}