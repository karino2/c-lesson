#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

int is_number_char(char c) {
    return c >= '0' && c <= '9';
}

int is_number(char *str) {
    return is_number_char(str[0]);
}

int parse_number(char *str) {
    int res = 0;
    while(is_number_char(*str)) {
        res *= 10;
        res += (*str++)-'0';
    }
    return res;
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

enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
};

int begin_with_len(struct Substr *in_str, char *expect, int expect_len) {
    int pos = 0;
    if(in_str->len > in_str->len)
        return 0;
    while(in_str->ptr[pos] == expect[pos] && pos < expect_len) {
        pos++;
    }
    return pos == expect_len;
}

int begin_with(struct Substr *in_str, char *expect) {
    return begin_with_len(in_str, expect, strlen(expect));
}

int parse_word(struct Substr *in_str) {
    if(begin_with(in_str, "add"))
        return OP_ADD;
    if(begin_with(in_str, "sub"))
        return OP_SUB;
    if(begin_with(in_str, "mul"))
        return OP_MUL;
    if(begin_with(in_str, "div"))
        return OP_DIV;

    fprintf(stderr, "Unknown word, %s\n", in_str->ptr);
    exit(1);
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

int stack_pos = 0;
int stack[1024];
void stack_push(int val) {
    stack[stack_pos++] = val;
}
int stack_pop() {
    if(stack_pos == 0) {
        fprintf(stderr, "stack pop while stack is empty, exit.\n");
        exit(1);
    }
    return stack[--stack_pos];
}

int eval(int r0, int r1, char *str) {    
    struct Substr remain={str, strlen(str)};
    int val;

    stack_pos = 0;

    while(!is_end(&remain)) {
        skip_space(&remain);
        if(is_number(remain.ptr)) {
            stack_push(parse_number(remain.ptr));
            skip_token(&remain);
            continue;
        }else if(is_register(remain.ptr)) {
            if(remain.ptr[1] == '1') {
                val = r1;
            } else {
                val = r0;
            }
            stack_push(r0);
            skip_token(&remain);
            continue;
        } else {
            // must be op.
            int arg1, arg2;

            val = parse_word(&remain);
            skip_token(&remain);

            arg1 = stack_pop();
            arg2 = stack_pop();

            switch(val) {
                case OP_ADD:
                    stack_push(arg1+arg2);
                    break;
                case OP_SUB:
                    stack_push(arg1-arg2);
                    break;
                case OP_MUL:
                    stack_push(arg1*arg2);                
                    break;
                case OP_DIV:
                    stack_push(arg1/arg2);
                    break;
            }
            continue;
        }
    }
    return stack_pop();
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

void test_parse_number() {
    assert_int_eq(123, parse_number("123"));
}

void test_begin_with() {
    struct Substr sub = {"def", 3};
    assert_true(begin_with(&sub, "de"));
    assert_true(begin_with(&sub, "def"));
    assert_false(begin_with(&sub, "deg"));
    assert_false(begin_with(&sub, "defg"));
    assert_false(begin_with(&sub, "very"));
}

void test_eval() {
    int actual;
    actual = eval(1, 5, "3 7 add r1 sub 4 mul r0 add");
    assert_int_eq(21, actual);    
}

void run_unit_tests() {
    test_skip_space_NotSpaceDoNothing();
    test_skip_space();

    test_is_XXX();

    test_skip_token();
    test_skip_token_TillEnd();
    test_skip_token_DoNothingWhenSpace();
    test_skip_token_DoNothingWhenEnd();

    test_parse_number();

    test_begin_with();

    test_eval();

    printf("all test done\n");
}


int main() {
    int res;
    int (*funcvar)(int, int);

    run_unit_tests();

    res = eval(1, 5, "3 7 add r1 sub 4 mul");
    printf("res=%d\n", res);
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

