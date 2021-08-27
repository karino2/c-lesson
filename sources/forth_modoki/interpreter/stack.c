#include "stack.h"
#include "parser.h"
#include "dict.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 1024

static Element stack[STACK_SIZE];

static int top = -1;

static int is_stack_empty() {
    return top < 0;
}

void stack_push(Element* element) {
    stack[++top] = *element;
}

void stack_pop(Element* out_element) {
    if (is_stack_empty()) return;
    *out_element = stack[top--];
}

void stack_peek(int depth, Element* out_element) {
    if (depth > top) return;
    *out_element = stack[top - depth];
}

void stack_print_all() {
    printf("--- stack ---\n");
    for (int i = top; i >= 0; i--) {
        Element e = stack[i];
        char buf[1024];
        stack_element_debug_string(buf, &e);

        printf("%d: %s\n", i, buf);
    }
    printf("-------------\n");
}
