#ifndef STACK_H
#define STACK_H

typedef enum {
    ET_NUMBER,
    ET_EXECUTABLE_NAME,
    ET_LITERAL_NAME,
} StackElementType;

typedef struct {
    StackElementType type;
    union {
        int number;
        char* name;
    } u;
} StackElement;

void stack_push(StackElement* element);

void stack_pop(StackElement* out_element);

#endif