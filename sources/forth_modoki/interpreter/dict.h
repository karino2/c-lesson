#ifndef DICT_H
#define DICT_H

#include "stack.h"

void eval_dict_put(char* key, Element* element);
int eval_dict_get(char* key, Element* out_element);
void eval_dict_clear();
void eval_dict_print_all();

void compile_dict_put(char* key, Element* element);
int compile_dict_get(char* key, Element* out_element);
void compile_dict_print_all();

#endif