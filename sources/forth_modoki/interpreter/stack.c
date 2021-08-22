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

void stack_peek(int depth, StackElement* out_element) {
    if (depth > top) return;
    *out_element = stack[top - depth];
}

void stack_print_all() {
    printf("--- stack ---\n");
    for (int i = top; i >= 0; i--) {
        StackElement e = stack[i];
        switch (e.type) {
        case ET_NUMBER:
            printf("%d: ET_NUMBER %d\n", i, e.u.number);
            break;
        case ET_EXECUTABLE_NAME:
            printf("%d: ET_EXECUTABLE_NAME %s\n", i, e.u.name);
            break;
        case ET_LITERAL_NAME:
            printf("%d: ET_LITERAL_NAME %s\n", i, e.u.name);
            break;
        case ET_C_FUNC:
            printf("%d: ET_C_FUNC\n", i);
            break;
        case ET_EXECUTABLE_ARRAY:
            printf("%d: ET_EXECUTABLE_ARRAY\n", i);
            break;
        }
    }
    printf("-------------\n");
}
