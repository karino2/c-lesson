#include <stdio.h>

char string_array[] = "This is array";
char *string_ptr = "This is pointer";

// for easier assembly reading
void print_string_array() {
    printf("%s\n", string_array);
}

void print_string_ptr() {
    printf("%s\n", string_ptr);
}

void print_sizeof() {
    printf("str array sizeof: %d\n", sizeof(string_array));
    printf("str ptr sizeof: %d\n", sizeof(string_ptr));
}

int main() {
    print_string_array();
    print_string_ptr();
    print_sizeof();

    return 0;
}

