#include <stdarg.h>


static char buf[100*1024];

static int to_buffer = 0;
static int pos = 0;

void cl_enable_buffer_mode() {
    to_buffer = 1;
}

char *cl_get_printed_buffer() {
    return buf;
}

void cl_printf(char *fmt, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, fmt);

    if(to_buffer) {
        pos += vsprintf(&buf[pos], fmt, arg_ptr);
        pos++;
    } else {
        vprintf(fmt, arg_ptr);
    }
    va_end(arg_ptr);
}


