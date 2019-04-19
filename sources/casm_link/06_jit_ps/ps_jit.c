#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "parser.h"
#include "test_util.h"

extern int eval(int r0, int r1, char *str);

/*
JIT
*/
int *binary_buf = NULL;

int* allocate_executable_buf(int size) {
    return (int*)mmap(0, size,
                 PROT_READ | PROT_WRITE | PROT_EXEC,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void ensure_jit_buf() {
    if(binary_buf == NULL) {
        binary_buf = allocate_executable_buf(1024);
    }
}

int* jit_script(char *input) {
    ensure_jit_buf();
    /*
    TODO: emit binary here
    */
    // dummy code to avoid crash.
    binary_buf[0] = 0xe1a0f00e; // 0xe1a0f00e

    return binary_buf;
}


static void run_unit_tests() {
    printf("all test done\n");
}


int main() {
    int res;
    int (*funcvar)(int, int);

    run_unit_tests();

    res = eval(1, 5, "3 7 add r1 sub 4 mul");
    printf("res=%d\n", res);

    /*
     TODO: Make below test pass.
    */
    funcvar = (int(*)(int, int))jit_script("3 7 add r1 sub 4 mul");

    res = funcvar(1, 5);
    assert_int_eq(20, res);

    res = funcvar(1, 4);
    assert_int_eq(24, res);

    return 0;
}

