#include <stdio.h>

void func1(char *str) {
    puts(str);
}

void print_something(char *str) {
    func1(str);
}

