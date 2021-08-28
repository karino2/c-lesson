#include "continuation.h"
#include <stdio.h>
#include <stdlib.h>

#define CO_STACK_SIZE 1024

static ContinuationElement co_stack[CO_STACK_SIZE];

static int top = -1;

ContinuationElement continuation(ElementArray* array, int pc) {
    Continuation* cont = malloc(sizeof(Continuation));
    cont->exec_array = array;
    cont->pc = pc;
    return (ContinuationElement) { CE_CONTINUATION, { .cont = cont } };
}
ContinuationElement local_var(Element* elem) {
    Element* copied = malloc(sizeof(Element));
    *copied = *elem;
    return (ContinuationElement) { CE_LOCAL_VAR, { .elem = copied } };
}

void co_stack_push(ContinuationElement* elem) {
    co_stack[++top] = *elem;
}

int co_stack_pop(ContinuationElement* out_elem) {
    if (top < 0) { return 0; }
    *out_elem = co_stack[top--];
    return 1;
}

int co_stack_pop_current_continuation(ContinuationElement* out_elem) {
    do {
        if (top < 0) { return 0; }
        *out_elem = co_stack[top--];
    } while (out_elem->type != CE_CONTINUATION);

    return out_elem->type == CE_CONTINUATION ? 1 : 0;
}

void co_stack_print_all() {
    printf("-- costack --\n");
    for (int i = top; i >= 0; i--) {
        ContinuationElement e = co_stack[i];
        switch (e.type) {
        case CE_LOCAL_VAR: {
            char buf[1024];
            stack_element_debug_string(buf, e.u.elem);

            printf("%d: %s\n", i, buf);
        }
        case CE_CONTINUATION: {
            char fbuf[1024], cbuf[1024];

            stack_element_debug_string(fbuf, &e.u.cont->exec_array->elements[0]);
            stack_element_debug_string(cbuf, &e.u.cont->exec_array->elements[e.u.cont->pc]);

            printf("%d: current(pc=%d/len=%d) [%s] first [%s] \n", i, e.u.cont->pc, e.u.cont->exec_array->len, cbuf, fbuf);
        }
        }
    }
    printf("-------------\n");
}
