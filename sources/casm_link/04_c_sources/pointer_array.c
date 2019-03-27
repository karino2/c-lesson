#include <stdio.h>

int intarray[1024];

struct FourStruct {
    int a1;
    int a2;
    int a3;
    int a4;
};

struct FourStruct structArray[1024];

void print_one(int val) {
    printf("%d\n", val);
}

void print_one_struct(struct FourStruct *ptr) {
    printf("%d, %d, %d, %d\n", ptr->a1, ptr->a2, ptr->a3, ptr->a4);
}


void iterate_array(int *array) {
    print_one(array[0]);
    print_one(array[1]);
    print_one(array[2]);
}

void iterate_array_ptr(int *array) {
    print_one(*(array++));
    print_one(*(array++));
    print_one(*(array++));
}

void iterate_struct_array_ptr(struct FourStruct *array) {
    print_one_struct(array++);
    print_one_struct(array++);
    print_one_struct(array++);
}

void write_pointer(int* ptr) {
    *(ptr++) = 123;
    *(ptr++) = 456;

}

void setup_arrays() {
    for(int i = 0; i < 1024; i++) {
        intarray[i] = 1024-i;
        structArray[i].a1 = i*1;
        structArray[i].a2 = i*2;
        structArray[i].a3 = i*3;
        structArray[i].a4 = i*4;
    }
}

int main() {
    setup_arrays();

    write_pointer(intarray);

    iterate_array(intarray);
    iterate_array_ptr(intarray);
    iterate_struct_array_ptr(structArray);

    return 0;
}

