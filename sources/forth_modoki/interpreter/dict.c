#include "dict.h"
#include "clesson.h"
#include <stdlib.h>

#define TABLE_SIZE 16

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

static Node* dict_array[TABLE_SIZE];

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
            head->value = *element;
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
        printf("%s: %d", node->key, node->value.u.number);
    }
}

void dict_put(char* key, StackElement* element) {
    int idx = hash(key);
    Node* head = dict_array[idx];
    if (head == NULL) {
        dict_array[idx] = new_node(key, element);
    }
    else {
        update_or_insert_list(head, key, element);
    }
}

int dict_get(char* key, StackElement* out_element) {
    int idx = hash(key);
    Node* node = dict_array[idx];
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

void dict_print_all() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* head = dict_array[i];
        if (head != NULL) {
            print_list(head);
        }
    }
}