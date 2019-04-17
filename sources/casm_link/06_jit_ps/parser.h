struct Substr {
    char *ptr;
    int len;
};


void skip_space(struct Substr *inout_str);
int is_number_char(char c);
int is_number(char *str);
int parse_number(char *str);
int is_register(char *str);
int is_end(struct Substr *in_str);

enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
};

/*
input: "add ..." or "sub ..." or "mul ..." or "div ...""
return: one of OP_ADD, OP_SUB, OP_MUL, OP_DIV
*/
int parse_word(struct Substr *in_str);

/*
skip below.
- 123
- r0
- add
*/
void skip_token(struct Substr *inout_str);
