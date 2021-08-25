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
        char buf[1024];
        stack_element_debug_string(buf, &e);

        printf("%d: %s\n", i, buf);
    }
    printf("-------------\n");
}

StackElement number_element(int number) {
    return (StackElement) { ET_NUMBER, { .number = number } };
}

StackElement ename_element(char* name) {
    return (StackElement) { ET_EXECUTABLE_NAME, { .name = name } };
}

StackElement lname_element(char* name) {
    return (StackElement) { ET_LITERAL_NAME, { .name = name } };
}

StackElement earray_element(StackElementArray* byte_codes) {
    return (StackElement) { ET_EXECUTABLE_ARRAY, { .byte_codes = byte_codes } };
}
void stack_element_debug_string(char* buf, StackElement* element) {
    switch (element->type) {
    case ET_NUMBER:
        sprintf(buf, "ET_NUMBER %d", element->u.number);
        break;
    case ET_EXECUTABLE_NAME:
        sprintf(buf, "ET_EXECUTABLE_NAME %s", element->u.name);
        break;
    case ET_LITERAL_NAME:
        sprintf(buf, "ET_LITERAL_NAME %s", element->u.name);
        break;
    case ET_C_FUNC:
        sprintf(buf, "C_FUNC");
        break;
    case ET_EXECUTABLE_ARRAY:
        sprintf(buf, "ET_EXECUTABLE_ARRAY");
        break;
    }
}