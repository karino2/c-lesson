#include <stdio.h>
#include <assert.h>

static const char* const input = "1203 456";


int _strlen(char *p)
{
    int len=0;

    while(p[len] != '\0' && p[len] != ' '){
        len++;
    }
    return len;
}


// 文字列の分割と部分文字列の長さを数える
int split(char **dst, int *dst_len, char *src){
    int count = 0;

    while(1) {
        while (*src == ' '){
            src++;
        }

        if (*src == '\0') break;

        dst_len[count] = _strlen(src);
        dst[count++] = src;

        while (*src && *src != ' ') {
            src++;
        }

        if (*src == '\0') break;

    }
    return count;
}


// 数字かどうかの判定
int _isdigit(char c)
{

    if (c >= '0' && c <= '9'){
        return 1;
    }else{
        return 0;
    }
}

// 複数桁の文字を数字の変換
int _atoi(char *s, int len)
{
    int i, n;
    i = 0;

    for(n=0; i <= len; i++){
        if(_isdigit(s[i])) {
            n = 10 * n + (s[i] - '0');
        }
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


    count = split(my_input_array, &my_len_arr, input);


    int numbers[100];
    for (int i = 0; i < count; ++i) {
        numbers[i] = _atoi(my_input_array[i], my_len_arr[i]);
    }

    answer1 = numbers[0];
    answer2 = numbers[1];


    // verity result.
    assert(answer1 == 1203);
    assert(answer2 == 456);

    return 1;
}