#ifndef STACK_H
#define STACK_H

typedef enum {
    ET_NUMBER,
    ET_EXECUTABLE_NAME,
    ET_LITERAL_NAME,
    ET_C_FUNC,
    ET_EXECUTABLE_ARRAY,
} StackElementType;

typedef struct StackElementArray StackElementArray;

typedef struct {
    StackElementType type;
    union {
        int number;
        char* name;
        void (*cfunc)();
        StackElementArray* byte_codes;
    } u;
} StackElement;

struct StackElementArray {
    int len;
    StackElement elements[0];
};

void stack_push(StackElement* element);

void stack_pop(StackElement* out_element);

void stack_peek(StackElement* out_element);

void stack_print_all();

#endif