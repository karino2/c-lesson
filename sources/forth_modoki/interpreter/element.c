#include "element.h"
#include "stdio.h"

Element number_element(int number) {
    return (Element) { ET_NUMBER, { .number = number } };
}

Element ename_element(char* name) {
    return (Element) { ET_EXECUTABLE_NAME, { .name = name } };
}

Element lname_element(char* name) {
    return (Element) { ET_LITERAL_NAME, { .name = name } };
}

Element earray_element(ElementArray* byte_codes) {
    return (Element) { ET_EXECUTABLE_ARRAY, { .byte_codes = byte_codes } };
}

Element primitive_element(Operation op) {
    return (Element) { ET_PRIMITIVE, { .op = op } };
}

void stack_element_debug_string(char* buf, Element* element) {
    switch (element->type) {
    case ET_NUMBER:
        sprintf(buf, "ET_NUMBER %d", element->u.number);
        break;
    case ET_EXECUTABLE_NAME:
        sprintf(buf, "ET_EXECUTABLE_NAME %s", element->u.name);
        break;
    case ET_LITERAL_NAME:
        sprintf(buf, "ET_LITERAL_NAME %s", element->u.name);
        break;
    case ET_C_FUNC:
        sprintf(buf, "C_FUNC");
        break;
    case ET_COMPILE_FUNC:
        sprintf(buf, "COMPILE_FUNC");
        break;
    case ET_PRIMITIVE:
        sprintf(buf, "PRIMITIVE");
    case ET_EXECUTABLE_ARRAY:
        sprintf(buf, "ET_EXECUTABLE_ARRAY");
        break;
    }
}
