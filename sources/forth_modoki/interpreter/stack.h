#ifndef STACK_H
#define STACK_H

#include "element.h"

void stack_push(Element* element);

void stack_pop(Element* out_element);

void stack_peek(int depth, Element* out_element);

void stack_print_all();

#endif