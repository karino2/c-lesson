#include <stdio.h>

void print_address(int address) {
    printf("address: %x\n", address);
}

int func3 (int a4) {
    // TODO: print func2's local avriable a3 here.

    return a4*3;
}

int func2(int a2) {
    int a3 = 0;
    for(int i = 0; i < 10; i++) {
        a3+=i;
    }
    return func3(a2+a3)+2;
}

int func1(int a1) {
    return func2(a1)+1;
}


int main() {
    int res = func1(123);
    printf("result is %d\n", res);
    return 0;
}

