/*
How to use : 


typedef struct {
    int value;
    float target_probability;
} MyNode;

void* create_my_node() {
    MyNode* node = (MyNode*)malloc(sizeof(MyNode));
    node->value = 0;
    node-> target_probability = 0.0;
    return node;
}

void delete_my_node(void* data) {
    free(data);
}

int main() {
    HashTable* table = create_hash_table(create_my_node, delete_my_node, sizeof(int), sizeof(int));

    int member_id = 1;
    int group_id = 1;
    MyNode* node = (MyNode*)get_or_create_node(table, &member_id, &group_id);
    node->value = 42;

    MyNode* retrieved_node = (MyNode*)get_or_create_node(table, &member_id, &group_id);
    printf("Node value: %d\n", retrieved_node->value); // Should print 42

    delete_hash_table(table);
    return 0;
}
*/

#include "two_way_hash_index.h"
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    void* member_id;
    void* group_id;
    void* data;
} Node;

struct HashTable {
    Node** member_id_index;
    Node** group_id_index;
    size_t member_id_size;
    size_t group_id_size;
    CreateNodeFunc create_node;
    DeleteNodeFunc delete_node;
    size_t capacity;
};

static size_t hash_function(const void* key, size_t key_size, size_t capacity) {
    size_t hash = 0;
    const char* data = (const char*)key;
    for (size_t i = 0; i < key_size; ++i) {
        hash = (hash * 31) + data[i];
    }
    return hash % capacity;
}

HashTable* create_hash_table(CreateNodeFunc create_node, DeleteNodeFunc delete_node, size_t member_id_size, size_t group_id_size) {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    table->capacity = 1024; // Initial capacity
    table->member_id_size = member_id_size;
    table->group_id_size = group_id_size;
    table->create_node = create_node;
    table->delete_node = delete_node;
    table->member_id_index = (Node**)calloc(table->capacity, sizeof(Node*));
    table->group_id_index = (Node**)calloc(table->capacity, sizeof(Node*));
    return table;
}

void delete_hash_table(HashTable* table) {
    for (size_t i = 0; i < table->capacity; ++i) {
        if (table->member_id_index[i]) {
            table->delete_node(table->member_id_index[i]->data);
            free(table->member_id_index[i]->member_id);
            free(table->member_id_index[i]->group_id);
            free(table->member_id_index[i]);
        }
    }
    free(table->member_id_index);
    free(table->group_id_index);
    free(table);
}

void* get_or_create_node(HashTable* table, const void* member_id, const void* group_id) {
    size_t member_hash = hash_function(member_id, table->member_id_size, table->capacity);
    size_t group_hash = hash_function(group_id, table->group_id_size, table->capacity);

    Node* node = table->member_id_index[member_hash];
    if (node && memcmp(node->member_id, member_id, table->member_id_size) == 0) {
        return node->data;
    }

    node = table->group_id_index[group_hash];
    if (node && memcmp(node->group_id, group_id, table->group_id_size) == 0) {
        return node->data;
    }

    node = (Node*)malloc(sizeof(Node));
    node->member_id = malloc(table->member_id_size);
    node->group_id = malloc(table->group_id_size);
    memcpy(node->member_id, member_id, table->member_id_size);
    memcpy(node->group_id, group_id, table->group_id_size);
    node->data = table->create_node();

    table->member_id_index[member_hash] = node;
    table->group_id_index[group_hash] = node;

    return node->data;
}