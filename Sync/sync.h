#ifndef SYNC_H
#define SYNC_H
#include "../linkedlist/linkedlist.h"
#include <time.h>

typedef enum {
    ADD = 1,
    DELETE,
    FIND,
    SHOW,
    FLUSH,
    UPDATE
}OPCODE;

typedef struct pack_{   //this data structure is for passing arguments to threads
        char data[32];
        char key[8];
    }pack_t;

int process_sync_msg(table_t *, char *, char *);
void hash_function(char*, char*);
void init_timer();
void start_timer();
void finish_timer();
double calculate_timer();
#endif
