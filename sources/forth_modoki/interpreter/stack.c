#include "stack.h"
#include <stdio.h>
#include <assert.h>

#define STACK_SIZE 1024

static StackElement stack[STACK_SIZE];

static int top = -1;

static int is_stack_empty() {
    return top < 0;
}

void stack_push(StackElement* element) {
    stack[++top] = *element;
}

void stack_pop(StackElement* out_element) {
    if (is_stack_empty()) return;
    *out_element = stack[top--];
}
