#include "stack.h"

typedef enum {
    CE_CONTINUATION,
    CE_LOCAL_VAR,
} ContinuationElementType;

typedef struct {
    ElementArray* exec_array;
    int pc;
} Continuation;

typedef struct {
    ContinuationElementType type;
    union {
        Element* elem;
        Continuation* cont;
    } u;
} ContinuationElement;

ContinuationElement continuation(ElementArray* array, int pc);
ContinuationElement local_var(Element* elem);

void co_stack_push(ContinuationElement* elem);

// return 1 if continuation found, otherwise return 0
int co_stack_pop(ContinuationElement* out_elem);

int co_stack_pop_current_continuation(ContinuationElement* out_elem);

int co_stack_peek(int depth, ContinuationElement* out_elem);

void co_stack_print_all();