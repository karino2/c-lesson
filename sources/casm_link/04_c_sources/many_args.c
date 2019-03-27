#include <stdio.h>

int many_sum(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13) {
    return 2*(a1+a2+a3+a4+a5+a6+a7+a8+a9+a10+a11+a12+a13);
}

int main() {
    int res = many_sum(13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
    printf("result is %d\n", res);
    return 0;
}

