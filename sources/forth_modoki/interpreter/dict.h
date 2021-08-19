#ifndef DICT_H
#define DICT_H

#include "stack.h"

void dict_put(char* key, StackElement* element);

int dict_get(char* key, StackElement* out_element);

void dict_print_all();

#endif