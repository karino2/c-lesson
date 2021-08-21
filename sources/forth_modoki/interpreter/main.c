#include "clesson.h"
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

        fseek(f, 0, SEEK_END);
        long length = ftell(f);
        fseek(f, 0, SEEK_SET);
        char* input = malloc(sizeof(char) * (length + 1));
        fread(input, sizeof(char), length, f);
        input[length] = '\0';

        fclose(f);

        cl_getc_set_src(input);

        register_primitives();
        printf("%d\n", eval());
        return 0;
    }
    default:
        printf("number of args should be 0 or 1\n");
        return 1;
    }
}
