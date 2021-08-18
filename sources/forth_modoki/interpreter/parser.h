typedef enum {
    LT_NUMBER,
    LT_SPACE,
    LT_EXECUTABLE_NAME,
    LT_LITERAL_NAME,
    LT_OPEN_CURLY,
    LT_CLOSE_CURLY,
    LT_END_OF_FILE,
    LT_UNKNOWN,
} LexicalType;

typedef struct {
    LexicalType ltype;
    union {
        int number;
        char onechar;
        char* name;
    } u;
} Token;

int parse_one(int prev_ch, Token* out_token);