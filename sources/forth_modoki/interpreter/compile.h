#ifndef COMPILE_H
#define COMPILE_H

#include "element.h"

#define MAX_NAME_OP_NUMBERS 256

struct Emitter {
    Element elems[MAX_NAME_OP_NUMBERS];
    int pos;
};

void register_compile_primitives();

int compile_exec_array(int prev_ch, Element* out_element);

#endif