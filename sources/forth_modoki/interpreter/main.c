#include "clesson.h"
#include "stack.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    switch (argc) {
    case 1:
        exec_tests();
        return 0;
    case 2: {
        FILE* f = fopen(argv[1], "r");
        if (!f) {
            printf("failed to open file\n");
            exit(1);
        }

        cl_getc_set_src_file(f);

        register_compile_primitives();
        register_primitives();
        eval();

        fclose(f);

        stack_print_all();
        return 0;
    }
    default:
        printf("number of args should be 0 or 1\n");
        return 1;
    }
}
