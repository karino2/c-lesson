#include <stdio.h>

int sum(int begin, int end) {
    int res = 0;
    for(int i = begin; i <= end; i++) {
        res+=i;
    }
    return res;
}

int main() {
    int res = sum(5, 12);
    printf("result is %d\n", res);
    return 0;
}

