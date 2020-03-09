#ifndef SYNC_H
#define SYNC_H

#include "../Hash-Table/hash-table.h"
#include "../data-List/data-list.h"

/* Synchronization protocol constants, structure, and API definitions */
#define SOCKET_NAME "NetworkAdminSocket"

#define WAIT 0
#define HASH_LIST 1
#define DATA_LIST 2

typedef struct dll_ dll_t;

typedef enum {
    CREATE,
    UPDATE,
    DELETE,
    NONE  // indicates that all current updates from server have been processed
} OPCODE;

/* Specifies whether we're dealing with hash table or data list */
typedef enum {
    HASH_L,
    DATA_L
} LCODE;

typedef struct _sync_msg {
    OPCODE op_code;
    LCODE l_code;
    union {
        hash_table_entry_t hash_table_entry;
        data_list_entry_t data_list_entry;
    } msg_body;
} sync_msg_t;

void process_sync_mesg(dll_t *dll, sync_msg_t *sync_msg);
extern int get_hash(const char *data, char *hash);

#endif
