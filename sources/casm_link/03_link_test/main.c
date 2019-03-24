#include <stdio.h>

extern void print_something(char *str);
extern char *g_text;
extern char *g_text_uninit;
int g_in_main = 2;

void func_in_main(char *str) {
    puts(str);
}


int main() {
    g_text_uninit = "set in main";
    print_something(g_text);
    return 0;
}

