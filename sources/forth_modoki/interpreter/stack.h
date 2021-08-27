#ifndef STACK_H
#define STACK_H

#define MAX_NAME_OP_NUMBERS 256

typedef struct Emitter Emitter;

typedef enum {
    ET_NUMBER,
    ET_EXECUTABLE_NAME,
    ET_LITERAL_NAME,
    ET_C_FUNC,
    ET_COMPILE_FUNC,
    ET_PRIMITIVE,
    ET_EXECUTABLE_ARRAY,
} StackElementType;

typedef struct StackElementArray StackElementArray;

typedef enum {
    OP_EXEC,
    OP_JMP,
    OP_JMP_NOT_IF,
} Operation;

typedef struct {
    StackElementType type;
    union {
        int number;
        char* name;
        Operation op;
        void (*cfunc)();
        void (*compfunc)(Emitter* em);
        StackElementArray* byte_codes;
    } u;
} StackElement;

struct StackElementArray {
    int len;
    StackElement elements[0];
};

void stack_push(StackElement* element);

void stack_pop(StackElement* out_element);

void stack_peek(int depth, StackElement* out_element);

void stack_print_all();

void stack_element_debug_string(char* buf, StackElement* element);

StackElement number_element(int number);
StackElement ename_element(char* name);
StackElement lname_element(char* name);
StackElement earray_element(StackElementArray* byte_codes);

struct Emitter {
    StackElement elems[MAX_NAME_OP_NUMBERS];
    int pos;
};

void register_compile_primitives();

int compile_exec_array(int prev_ch, StackElement* out_element);

#endif