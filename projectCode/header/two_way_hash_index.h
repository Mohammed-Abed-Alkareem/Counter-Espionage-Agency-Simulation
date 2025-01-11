#ifndef TWO_WAY_HASH_INDEX_H
#define TWO_WAY_HASH_INDEX_H

#include <stddef.h>

typedef struct HashTable HashTable;

typedef void* (*CreateNodeFunc)(void);
typedef void (*DeleteNodeFunc)(void*);

HashTable* create_hash_table(CreateNodeFunc create_node, DeleteNodeFunc delete_node, size_t member_id_size, size_t group_id_size);
void delete_hash_table(HashTable* table);

void* get_or_create_node(HashTable* table, const void* member_id, const void* group_id);

#endif // TWO_WAY_HASH_INDEX_H