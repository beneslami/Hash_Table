#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data-list.h"
#include "../DLL/dll.h"
#include "../Hash-Table/hash-table.h"

extern int get_hash(const char *data, char *hash);

/* Display each row (entry) of a hash table. */
void display_data_list(const dll_t *data_list) {
    printf("Printing data list\n");

    dll_node_t *node = data_list->head->next;
    while (node != data_list->head) {
        data_list_entry_t entry = *((data_list_entry_t *) node->data);
        printf("data: %s ", entry.data);

        char hash[HASH_LEN];
        if (get_hash(entry.data, hash) != -1) {
            printf("hash: %s", hash);
        }
        printf("\n");
        node = node->next;
    }
}

/* Look up entry in data list by data. */
dll_node_t *find_data(const dll_t *data_list, const char *data) {
    dll_node_t *node = data_list->head->next;
    while (node != data_list->head) {
        data_list_entry_t entry = *((data_list_entry_t *) node->data);
        if (!strcmp(entry.data, data)) {
            break;
        }
        node = node->next;
    }
    return node;
}
