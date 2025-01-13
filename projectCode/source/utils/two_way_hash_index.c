/*
How to use : 

typedef struct {
    int value;
    float target_probability;
} MyNode;

void* create_my_node() {
    MyNode* node = (MyNode*)malloc(sizeof(MyNode));
    node->value = 0;
    node->target_probability = 0.0;
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
    struct Node* next_member;
} Node;

typedef struct GroupNode {
    void* group_id;
    Node* members;
    struct GroupNode* next_group;
} GroupNode;

struct HashTable {
    GroupNode** group_id_index;
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
    table->group_id_index = (GroupNode**)calloc(table->capacity, sizeof(GroupNode*));
    return table;
}

void delete_hash_table(HashTable* table) {
    for (size_t i = 0; i < table->capacity; ++i) {
        GroupNode* group_node = table->group_id_index[i];
        while (group_node) {
            Node* member_node = group_node->members;
            while (member_node) {
                Node* temp_member = member_node;
                member_node = member_node->next_member;
                table->delete_node(temp_member->data);
                free(temp_member->member_id);
                free(temp_member);
            }
            GroupNode* temp_group = group_node;
            group_node = group_node->next_group;
            free(temp_group->group_id);
            free(temp_group);
        }
    }
    free(table->group_id_index);
    free(table);
}

void* get_or_create_node(HashTable* table, const void* member_id, const void* group_id) {
    size_t group_hash = hash_function(group_id, table->group_id_size, table->capacity);

    GroupNode* group_node = table->group_id_index[group_hash];
    while (group_node && memcmp(group_node->group_id, group_id, table->group_id_size) != 0) {
        group_node = group_node->next_group;
    }

    if (!group_node) {
        group_node = (GroupNode*)malloc(sizeof(GroupNode));
        group_node->group_id = malloc(table->group_id_size);
        memcpy(group_node->group_id, group_id, table->group_id_size);
        group_node->members = NULL;
        group_node->next_group = table->group_id_index[group_hash];
        table->group_id_index[group_hash] = group_node;
    }

    Node* member_node = group_node->members;
    while (member_node && memcmp(member_node->member_id, member_id, table->member_id_size) != 0) {
        member_node = member_node->next_member;
    }

    if (!member_node) {
        member_node = (Node*)malloc(sizeof(Node));
        member_node->member_id = malloc(table->member_id_size);
        memcpy(member_node->member_id, member_id, table->member_id_size);
        member_node->data = table->create_node();
        member_node->next_member = group_node->members;
        group_node->members = member_node;
    }

    return member_node->data;
}

// function that will iterate over the group members and call the function on each member
void iterate_group_members(HashTable* table, const void* group_id, void (*func)(void* data)) {
    size_t group_hash = hash_function(group_id, table->group_id_size, table->capacity);

    GroupNode* group_node = table->group_id_index[group_hash];
    while (group_node && memcmp(group_node->group_id, group_id, table->group_id_size) != 0) {
        group_node = group_node->next_group;
    }

    if (group_node) {
        Node* member_node = group_node->members;
        while (member_node) {
            func(member_node->data);
            member_node = member_node->next_member;
        }
    }
}

//example on how to use the iterate_group_members function
// void print_data(void* data) {
//     MyNode* node = (MyNode*)data;
//     printf("Node value: %d\n", node->value);
// }

// int main() {
//     HashTable* table = create_hash_table(create_my_node, delete_my_node, sizeof(int), sizeof(int));
//     int group_id = 1;
//     iterate_group_members(table, &group_id, print_data);
//     delete_hash_table(table);
