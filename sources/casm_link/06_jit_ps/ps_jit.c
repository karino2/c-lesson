#include <stdio.h>
#include <sys/mman.h>


/*
1 2 add r1 sub 4 mul
*/

struct Substr {
    char *ptr;
    int len;
};

void goto_forward(struct Substr *inout_str, int delta) {
    inout_str->ptr = inout_str->ptr+delta;
    inout_str->len -= delta;
}

void skip_space(struct Substr *inout_str) {
    int newbeg = 0;
    while(inout_str->ptr[newbeg] == ' ') newbeg++;
    goto_forward(inout_str, newbeg);
}

int is_number(char *str) {
    return (str[0] >= '0' && str[0] <= '9');
}

int is_register(char *str) {
    if(str[0] == 'r') {
        return (str[1] == '0' || str[1] == '1');
    }
    return 0;
}

int is_end(struct Substr *in_str) {
    return in_str->len == 0;
}

/*
skip below.
- 123
- r0
- add
*/
void skip_token(struct Substr *inout_str) {
    int newbeg = 0;
    while(newbeg < inout_str->len && inout_str->ptr[newbeg] != ' ') { newbeg++; }
    if(newbeg == inout_str->len) {
        inout_str->ptr = NULL;
        inout_str->len = 0;
        return;
    }
    goto_forward(inout_str, newbeg);
}


/*
JIT
*/
int *binary_buf;

int* allocate_executable_buf(int size) {
    return (int*)mmap(0, size,
                 PROT_READ | PROT_WRITE | PROT_EXEC,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}



/*
test code
*/


void assert_true(int boolflag) {
    if(!boolflag) {
        printf("assert fail\n");
    }
}

void assert_false(int boolflag) {
    assert_true(!boolflag);
}

void assert_int_eq(int expect, int actual) {
    if(expect != actual) {
        printf("assert fail\n");
    }
}


void test_skip_space_NotSpaceDoNothing() {
    struct Substr sub = {"abc", 3};
    char *expect_ptr = sub.ptr;
    skip_space(&sub);

    assert_true(expect_ptr == sub.ptr);
    assert_int_eq(3, sub.len);
}

void test_skip_space() {
    struct Substr sub = {"  abc  ", 7};
    char *expect_ptr = sub.ptr+2;
    skip_space(&sub);

    assert_true(expect_ptr == sub.ptr);
    assert_true(sub.ptr[0] == 'a');
    assert_int_eq(5, sub.len);
}

void test_is_XXX() {
    assert_true(is_number("123"));
    assert_false(is_number("a123"));

    assert_false(is_register("a123"));
    assert_false(is_register("123"));
    assert_true(is_register("r0"));
    assert_true(is_register("r1"));
}

void test_skip_token() {
    struct Substr sub = {"abc def", 7};
    char *expect_ptr = sub.ptr+3;

    skip_token(&sub);

    assert_true(expect_ptr == sub.ptr);
    assert_int_eq(4, sub.len);
    assert_true(sub.ptr[0] == ' ');
}

void test_skip_token_DoNothingWhenSpace() {
    struct Substr sub = {" def", 4};
    char *expect_ptr = sub.ptr;

    skip_token(&sub);

    assert_true(expect_ptr == sub.ptr);
    assert_int_eq(4, sub.len);
    assert_true(sub.ptr[0] == ' ');
}

void test_skip_token_TillEnd() {
    struct Substr sub = {"def", 3};

    skip_token(&sub);

    assert_true(NULL == sub.ptr);
    assert_int_eq(0, sub.len);
}


void test_skip_token_DoNothingWhenEnd() {
    struct Substr sub = { NULL, 0};

    skip_token(&sub);

    assert_true(NULL == sub.ptr);
    assert_int_eq(0, sub.len);
}

void run_unit_tests() {
    test_skip_space_NotSpaceDoNothing();
    test_skip_space();

    test_is_XXX();

    test_skip_token();
    test_skip_token_TillEnd();
    test_skip_token_DoNothingWhenSpace();
    test_skip_token_DoNothingWhenEnd();

    printf("all test done\n");
}


int main() {
    int res;
    int (*funcvar)(int, int);

    run_unit_tests();
    /*

    res = sum_till(10);
    assert_true(res == 45);

    res = sum_till_inline(10);
    assert_true(res == 45);
    */

    /* TODO: Pass below code.
    */
   /*
    binary_buf = allocate_executable_buf(1024);

    jit_sum_till();

    funcvar = (int(*)(int))binary_buf;
    res = funcvar(10);
    assert_true(res == 45);

    res = funcvar(11);
    assert_true(res == 55);
    */

    return 0;
}

