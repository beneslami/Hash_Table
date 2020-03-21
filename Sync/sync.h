#ifndef SYNC_H
#define SYNC_H
#include "../linkedlist/linkedlist.h"

typedef enum {
    ADD = 1,
    DELETE,
    FIND,
    SHOW,
    FLUSH,
    UPDATE
}OPCODE;

typedef struct pack_{
        char data[32];
        char hash[32];
    }pack_t;
    
int process_sync_msg(table_t *, char *);

#endif