#ifndef STACK_H
#define STACK_H

typedef enum {
    ET_NUMBER,
    ET_EXECUTABLE_NAME,
    ET_LITERAL_NAME,
    ET_C_FUNC,
} StackElementType;

typedef struct {
    StackElementType type;
    union {
        int number;
        char* name;
        void (*cfunc)();
    } u;
} StackElement;

void stack_push(StackElement* element);

void stack_pop(StackElement* out_element);

void stack_print_all();

#endif