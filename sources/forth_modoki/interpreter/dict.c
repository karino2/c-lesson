#include "dict.h"
#include "clesson.h"

typedef struct {
    char* key;
    StackElement value;
} KeyValue;

static KeyValue dict_array[1024];

static int dict_pos = 0;

static int dict_index(char* key) {
    for (int i = 0; i < dict_pos; i++) {
        if (streq(key, dict_array[i].key)) {
            return i;
        }
    }
    return -1;
}

void dict_put(char* key, StackElement* element) {
    KeyValue keyValue = { key, *element };

    int index = dict_index(key);
    if (index >= 0) {
        dict_array[index] = keyValue;
    }
    else {
        dict_array[dict_pos++] = keyValue;
    }
}

int dict_get(char* key, StackElement* out_element) {
    int index = dict_index(key);
    if (index >= 0) {
        *out_element = dict_array[index].value;
        return 1;
    }
    else {
        return 0;
    }
}

void dict_print_all() {
    for (int i = 0; i < dict_pos; i++) {
        if (dict_array[i].value.type != ET_NUMBER) {
            printf("%s is assigned to unexpected type %d", dict_array[i].key, dict_array[i].value.type);
            continue;
        }
        printf("%s: %d", dict_array[i].key, dict_array[i].value.u.number);
    }
}