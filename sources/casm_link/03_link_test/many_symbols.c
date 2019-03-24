
extern int g_in_main;
int g_in_hello_uninit;
int g_in_hello = 1;
static int g_static_in_hello=1;
static int g_static_uninit;
char *g_text = "abc";
char *g_text_uninit;
char g_text_arr[] = "def";
int g_large_buf[1024*1024];


extern void func_in_main(char *str);


int print_something(char *str) {
    func_in_main(str);
    func_in_main(g_text_uninit);
    return g_in_main;
}

