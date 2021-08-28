#ifndef ELEMENT_H
#define ELEMENT_H

struct Emitter;
typedef struct Emitter Emitter;

typedef enum {
    ET_NUMBER,
    ET_EXECUTABLE_NAME,
    ET_LITERAL_NAME,
    ET_C_FUNC,
    ET_COMPILE_FUNC,
    ET_PRIMITIVE,
    ET_EXECUTABLE_ARRAY,
} ElementType;

typedef struct ElementArray ElementArray;

typedef enum {
    OP_EXEC,
    OP_JMP,
    OP_JMP_NOT_IF,
    OP_STORE,
    OP_LOAD,
} Operation;

typedef struct {
    ElementType type;
    union {
        int number;
        char* name;
        Operation op;
        void (*cfunc)();
        void (*compfunc)(Emitter* em);
        ElementArray* byte_codes;
    } u;
} Element;

struct ElementArray {
    int len;
    Element elements[0];
};

Element number_element(int number);
Element ename_element(char* name);
Element lname_element(char* name);
Element earray_element(ElementArray* byte_codes);
Element primitive_element(Operation op);

void stack_element_debug_string(char* buf, Element* element);

#endif