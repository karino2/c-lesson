#include "clesson.h"
#include <string.h>

static const char* input = "123 456";
static int pos = 0;


int cl_getc() {
    if(strlen(input) == pos)
        return EOF;
    return input[pos++];
}

void cl_getc_set_src(char* str){
    input = str;
    pos = 0;
}
