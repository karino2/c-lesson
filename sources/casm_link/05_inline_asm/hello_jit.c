#include <stdio.h>
#include <sys/mman.h>

int return5_inline() {
    int res;
    asm("mov %0, #5" :"=r"(res));
    return res;
}

int *binary_buf;

int* allocate_executable_buf(int size) {
    return (int*)mmap(0, size,
                 PROT_READ | PROT_WRITE | PROT_EXEC,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

/*
test code
*/


void assert_true(int boolflag) {
    if(!boolflag) {
        printf("assert fail\n");
    }
}


int main() {
    int res;
    int (*funcvar)();

    
    res = return5_inline();
    assert_true(res == 5);

    /* --- generate function --- */
    // allocate executable memory.
    binary_buf = allocate_executable_buf(1024);

    // mov r0, #5
    binary_buf[0] = 0xe3a00005;
    // mov r15, r14
    binary_buf[1] = 0xe1a0f00e;


    // convert buf to function!
    funcvar = (int(*)())binary_buf;

    res = funcvar();
    assert_true(res == 5);

    return 0;
}

