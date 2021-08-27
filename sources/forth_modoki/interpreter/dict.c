#include "dict.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define TABLE_SIZE 16

static int streq(char* s1, char* s2) {
    return strcmp(s1, s2) == 0;
}

static int hash(char* s) {
    unsigned int hashed = 0;
    while (*s) hashed += *s++;

    return (int)(hashed % TABLE_SIZE);
}

typedef struct Node Node;
struct Node {
    char* key;
    StackElement value;
    Node* next;
};

static Node* eval_dict_array[TABLE_SIZE];
static Node* compile_dict_array[TABLE_SIZE];

static Node* new_node(char* key, StackElement* element) {
    Node* node = malloc(sizeof(Node));

    node->key = malloc(sizeof(*key) * (strlen(key) + 1));
    strcpy(node->key, key);
    node->value = *element;
    node->next = NULL;

    return node;
}

static void update_or_insert_list(Node* head, char* key, StackElement* element) {
    Node* node = head;
    while (1) {
        if (streq(key, node->key)) {
            // 同じ key を持つ要素が見つかったら update する
            node->value = *element;
            return;
        }
        else if (node->next == NULL) {
            // リストの末尾まで同じ key を持つ要素が見つからなかったら insert する
            node->next = new_node(key, element);
            return;
        }

        node = node->next;
    }
}

static void print_list(Node* head) {
    Node* node = head;
    while (node != NULL) {
        StackElement e = node->value;
        switch (e.type) {
        case ET_NUMBER:
            printf("%s: %d\n", node->key, e.u.number);
            break;
        case ET_C_FUNC:
            // 事前に登録してある C_FUNC は表示をスキップする
            break;
        case ET_COMPILE_FUNC:
            // 事前に登録してある COMPILE_FUNC は表示をスキップする
            break;
        case ET_EXECUTABLE_ARRAY:
            printf("%s: EXECUTABLE_ARRAY\n", node->key);
            break;
        default:
            printf("unexpected element type of dict: %d\n", e.type);
            exit(1);
        }

        node = node->next;
    }
}

static void dict_put(Node* dict[], char* key, StackElement* element) {
    int idx = hash(key);
    Node* head = dict[idx];
    if (head == NULL) {
        dict[idx] = new_node(key, element);
    }
    else {
        update_or_insert_list(head, key, element);
    }
}

void eval_dict_put(char* key, StackElement* element) {
    dict_put(eval_dict_array, key, element);
}

void compile_dict_put(char* key, StackElement* element) {
    dict_put(compile_dict_array, key, element);
}

int dict_get(Node* dict[], char* key, StackElement* out_element) {
    int idx = hash(key);
    Node* node = dict[idx];
    if (node == NULL) {
        return 0;
    }
    else {
        while (node != NULL) {
            if (streq(key, node->key)) {
                *out_element = node->value;
                return 1;
            }
            node = node->next;
        }
        return 0;
    }
}

int eval_dict_get(char* key, StackElement* out_element) {
    return dict_get(eval_dict_array, key, out_element);
}

int compile_dict_get(char* key, StackElement* out_element) {
    return dict_get(compile_dict_array, key, out_element);
}

void eval_dict_clear() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        eval_dict_array[i] = NULL;
    }
}

void eval_dict_print_all() {
    printf("--- dict ----\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* head = eval_dict_array[i];
        if (head != NULL) {
            print_list(head);
        }
    }
    printf("-------------\n");
}

void compile_dict_print_all() {
    printf("--- dict ----\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* head = compile_dict_array[i];
        if (head != NULL) {
            print_list(head);
        }
    }
    printf("-------------\n");
}