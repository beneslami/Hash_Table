#ifndef SYNC_H
#define SYNC_H

typedef enum {
    ADD = 1,
    DELETE,
    FIND,
    SHOW,
    FLUSH
}OPCODE;

int create_sync_msg();

#endif