#include <stdio.h>
#include <assert.h>

static const char* input = "123 456";

// 文字列のコピー
void _strcpy(char *s, char *t){
    int i = 0;

    while((s[i] = t[i]) != '\0') // \0までポインタ同士を代入していく
        i++;
}


# 文字列の分割
int split(char *dst[], int*dst_len, char *src, char delim){
    int count = 0;

    while(1) {
        while (*src == delim){
            src++;
        }

        if (*src == '\0') break;

        dst[count++] = src;
        while (*src && *src != delim) {
            src++;
        }

        if (*src == '\0') break;

        *src++ = '\0';
    }
    return count;
}


# 文字と数字の変換
int _isdigit(char c)
{
    int num;

    if (c >= '0' && c <= '9'){
        num = c - '0';
    }else{
        num = 0;
    }
    return num;
}

# 複数桁の文字を数字の変換
int _atoi(char s[])
{
    int i, n;
    i = 0;

    for(n=0; _isdigit(s[i]); i++){
        n = 10 * n + (s[i] - '0');
    }
    return n;
}


int main() {
    int answer1 = 0;
    int answer2 = 0;

    // write something here.

    char *my_input_array[100];
    int count;
    int my_len_arr[100];

    int numbers[100];

    char my_input[100];
    _strcpy(my_input, input);

    count = split(my_input_array, my_len_arr, my_input, ' ');

    for (int i = 0; i < count; ++i) {
        numbers[i] = _atoi(my_input_array[i]);
    }

    answer1 = numbers[0];
    answer2 = numbers[1];


    // verity result.
    assert(answer1 == 123);
    assert(answer2 == 456);

    return 1;
}