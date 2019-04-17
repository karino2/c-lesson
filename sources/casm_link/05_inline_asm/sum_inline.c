#include <stdio.h>

int sum_till(int a) {
    int sum=0;
    for(int i = 0; i < a; i++) {
        sum+=i;
    }
    return sum;
}

int sum_till_inline(int a) {
    int res;
    /*
    Use r1 as loop counter.
    Use r2 to store temp result.
    */
    asm("mov r1, #0");
    asm("mov r2, #0");
    asm("loop:");
    asm("cmp r0, r1");
    asm("beq end");
    asm("add r2, r2, r1"); // sum+=i
    asm("add r1, r1, #1"); // i++    
    asm("b loop");
    asm("end:");
    asm("mov %0, r2" :"=r"(res));
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
    
    res = sum_till(10);
    assert_true(res == 45);

    res = sum_till_inline(10);
    assert_true(res == 45);

    /* TODO: Pass below code.
    */


    return 0;
}

