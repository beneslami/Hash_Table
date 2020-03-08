#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash-table.h"
#include "../DLL/dll.h"

/* Display each row (entry) of a hash table. */
void display_hash_table(const dll_t *hash_table) {
    printf("Printing hash table\n");
    dll_node_t *node = hash_table->head->next;
    while (node != hash_table->head) {
        hash_table_entry_t entry = *((hash_table_entry_t *) node->data);
        printf("hash: %s\n", entry.hash);
        node = node->next;
    }
}

/* Look up entry in hash table by data. */
dll_node_t *find_hash_table_entry(const dll_t *hash_table, const char *dest) {
    dll_node_t *node = hash_table->head->next;
    while (node != hash_table->head) {
        hash_table_entry_t entry = *((hash_table_entry_t *) node->data);
        if (!strcmp(entry.hash, dest)) {
            break;
        }
        node = node->next;
    }
    return node;
}

/* Updates the entry whose hash value is dest */
void update(dll_node_t *node, const char *dest) {
    hash_table_entry_t *entry = node->data;
    memset(entry->hash, 0, HASH_LEN);
}
