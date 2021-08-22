#include "clesson.h"
#include <string.h>
#include <stdlib.h>

static const char* input = NULL;
static int pos = 0;

static FILE* file = NULL;

static int cl_getc_str() {
    if (strlen(input) == pos)
        return EOF;
    return input[pos++];
}

static int cl_getc_file() {
    return fgetc(file);
}

int cl_getc() {
    if (input != NULL) {
        return cl_getc_str();
    }
    else if (file != NULL) {
        return cl_getc_file();
    }
    else {
        printf("cl_getc source not set\n");
        exit(1);
    }
}

void cl_getc_set_src(char* str) {
    input = str;
    pos = 0;
}

void cl_getc_set_src_file(FILE* f) {
    file = f;
}
