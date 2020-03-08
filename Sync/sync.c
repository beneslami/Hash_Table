#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/mman.h> // for unlink
#include <errno.h>

#include "../DLL/dll.h"
#include "sync.h"

/* Makes any changes to a hash table or data list based on the instructions encoded in sync_msg. */
void process_sync_mesg(dll_t *dll, sync_msg_t *sync_msg) {
    dll_node_t *node;
    if (sync_msg->l_code == HASH_L) {
        node = find_hash_table_entry(dll, sync_msg->msg_body.hash_table_entry.hash);
        switch (sync_msg->op_code) {
            case CREATE:
                if (node == dll->head) {
                    append(dll, &sync_msg->msg_body.hash_table_entry);
                    node = find_hash_table_entry(dll, sync_msg->msg_body.hash_table_entry.hash);
                    if (node != dll->head) {
                        hash_table_entry_t entry = *((hash_table_entry_t *) node->data);
                        printf("Added hash: %s\n", entry.hash);
                    }
                }
                break;
            case UPDATE:
                if (node != dll->head) {
                    update(node, sync_msg->msg_body.hash_table_entry.hash);
                    node = find_hash_table_entry(dll, sync_msg->msg_body.hash_table_entry.hash);
                    if (node != dll->head) {
                        hash_table_entry_t entry = *((hash_table_entry_t *) node->data);
                        printf("Updated hash: %s\n", entry.hash);
                    }
                }
                break;
            case DELETE:
                if (node != dll->head) {
                    del(dll, node);
                    node = find_hash_table_entry(dll, sync_msg->msg_body.hash_table_entry.hash);
                    if (node == dll->head) {
                        printf("Deleted hash: %s\n", sync_msg->msg_body.hash_table_entry.hash);
                    }
                }
                break;
            default:
                break;
        }
    }
    else {
        node = find_data(dll, sync_msg->msg_body.data_list_entry.data);
        switch (sync_msg->op_code) {
            case CREATE:
                if (node == dll->head) {
                    append(dll, &sync_msg->msg_body.data_list_entry);
                    node = find_data(dll, sync_msg->msg_body.data_list_entry.data);
                    if (node != dll->head) {
                        data_list_entry_t entry = *((data_list_entry_t *) node->data);
                        printf("Added data: %s", entry.data);
                        char hash[HASH_LEN];
                        if (get_hash(entry.data, hash) != -1) {
                            printf("hash: %s", hash);
                        }
                        printf("\n");
                    }
                }
                break;
            case DELETE:
                if (node != dll->head) {
                    del(dll, node);
                    node = find_data(dll, sync_msg->msg_body.data_list_entry.data);
                    if (node == dll->head) {
                        printf("Deleted data: %s\n", sync_msg->msg_body.data_list_entry.data);
                        unlink(sync_msg->msg_body.data_list_entry.data); // deallocate shared memory region corresponding to this data
                    }
                }
                break;
            default:
                break;
        }
    }
}
