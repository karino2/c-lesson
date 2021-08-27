#ifndef CL_H
#define CL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
return one character and move cursor.
return EOF if end of file.
*/
int cl_getc();
void cl_getc_set_src(char* str);
void cl_getc_set_src_file(FILE* f);

#endif
