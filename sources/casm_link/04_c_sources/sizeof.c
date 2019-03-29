#include <stdio.h>
#include <stdlib.h>

struct SomeStruct {
    int a;
    double b;
    int c;
};


// for easier assembly reading.
void print_size(int size) {
    printf("size: %d\n", size);
}

void array_size() {
    int array[1024];
    int res;

    res = sizeof(array);
    print_size(res);
}

void ptr_size() {
    int *ptr;
    int res;

    ptr = malloc(1024);

    res = sizeof(ptr);
    print_size(res);

    free(ptr);
}

void struct_size() {
    struct SomeStruct struc;
    int res;

    res = sizeof(struc);
    print_size(res);

    res = sizeof(struct SomeStruct);
    print_size(res);
}

void var_array(int size) {
    int array[size];
    int res = sizeof(array);
    print_size(res);
}

int main() {

    array_size();
    ptr_size();
    struct_size();
    var_array(123);

    return 0;
}

