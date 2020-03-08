#ifndef HASHTABLE_H
#define HASHTABLE_H

#define HASH_LEN 32

typedef struct dll_ dll_t;
typedef struct dll_node_ dll_node_t;

/* Data structure definition for hash table entry */
typedef struct hash_table_entry_ {
    char hash[HASH_LEN];
} hash_table_entry_t;

/* Public API's for hash table functionality */
void display_hash_table(const dll_t *hash_table);
dll_node_t *find_hash_table_entry(const dll_t *hash_table, const char *dest);
void update(dll_node_t *node, const char *dest);

#endif
