#include "stack.h"
#include "parser.h"
#include "dict.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

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

static void emit(Emitter* em, StackElement elem) {
    em->elems[em->pos++] = elem;
}

static void compile_ifelse(Emitter* em) {
    emit(em, number_element(3));
    emit(em, number_element(2));
    emit(em, ename_element("roll"));
    emit(em, number_element(5));
    emit(em, ename_element("jmp_not_if"));
    emit(em, ename_element("pop"));
    emit(em, ename_element("exec"));
    emit(em, number_element(4));
    emit(em, ename_element("jmp"));
    emit(em, ename_element("exch"));
    emit(em, ename_element("pop"));
    emit(em, ename_element("exec"));
}

int compile_exec_array(int prev_ch, StackElement* out_element) {
    int ch = prev_ch;
    Token token = {
        LT_UNKNOWN,
        {0}
    };

    int i = 0;
    Emitter em;
    em.pos = 0;

    do {
        ch = parse_one(ch, &token);

        if (token.ltype != LT_UNKNOWN) {
            switch (token.ltype) {
            case LT_NUMBER:
                emit(&em, number_element(token.u.number));
                break;
            case LT_EXECUTABLE_NAME:
                if (streq(token.u.name, "ifelse")) {
                    compile_ifelse(&em);
                }
                else {
                    emit(&em, ename_element(token.u.name));
                }
                break;
            case LT_LITERAL_NAME:
                emit(&em, lname_element(token.u.name));
                break;
            case LT_OPEN_CURLY: {
                StackElement e;
                ch = compile_exec_array(ch, &e);
                emit(&em, e);
                break;
            }
            case LT_CLOSE_CURLY:
            case LT_SPACE:
                break;
            default:
                break;
            }
        }
    } while (token.ltype != LT_END_OF_FILE && token.ltype != LT_CLOSE_CURLY);

    StackElementArray* array = malloc(sizeof(StackElementArray) + sizeof(StackElement) * em.pos);
    array->len = em.pos;
    memcpy(array->elements, em.elems, sizeof(StackElement) * em.pos);
    *out_element = earray_element(array);

    return ch;
}