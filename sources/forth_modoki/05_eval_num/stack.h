#include <stdlib.h>
#include <string.h>

typedef enum {
    ET_NUMBER,
    ET_EXECUTABLE_NAME,
} StackElementType;

typedef struct {
    StackElementType type;
    union {
        int number;
        char* name;
    } u;
} StackElement;

void stack_push(StackElement*);

StackElement* stack_pop();
