#include <stdio.h>

int addmul3(int a, int b) {
    return (a+b)*3;
}

int addmul3_inline_notwork(int a, int b) {
    asm("mov r2, r0");
    asm("add r2, r2, r1");
    asm("mov r3, #3");
    asm("mul r0, r2, r3");
}

int addmul3_inline(int a, int b) {
    int res;
    asm("mov r2, r0");
    asm("add r2, r2, r1");
    asm("mov r3, #3");
    asm("mul %0, r2, r3" :"=r"(res));
    return res;
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
    
    res = addmul3(3, 4);
    assert_true(res == 21);

    res = addmul3_inline(3, 4);
    assert_true(res == 21);


    return 0;
}

