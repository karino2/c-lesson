#include <stdio.h>

void print_address(int address) {
    printf("address: %x\n", address);
}

// #define INVESTIGATE 1
#undef INVESTIGATE

int func3 (int a4) {
#ifdef INVESTIGATE
    // &a4 +4 == org_sp, func2's r11 address.
    print_address(((int)&a4)+ 4);

    // func2 r11 val
    print_address( *((int*)(((int)&a4)+ 4)));

    // &a3
    print_address( *((int*)(((int)&a4)+ 4)) -8);
 
    // *(&a3) == a3
    print_address(*((int*)(*((int*)(((int)&a4)+ 4)) -8)));
#else
    printf("We get a3 value, %d\n", *((int*)(*((int*)(((int)&a4)+ 4)) -8)));
#endif
    return a4*3;
}

int func2(int a2) {
    int a3 = 0;
    for(int i = 0; i < 10; i++) {
        a3+=i;
    }
#ifdef INVESTIGATE
    printf("func2's a3 address = %x\n", (int) &a3);
#endif
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

