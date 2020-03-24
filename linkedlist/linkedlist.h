#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct data_entry_{
    char data[32];
    struct data_entry_ *next;
}data_entry_t;

typedef struct table_entry_{
    char hash[32];
    struct data_entry_ *next_data;
    struct table_entry_ *next_hash;
}table_entry_t;

typedef struct table_{
    table_entry_t *next;
}table_t;
table_t *table;

table_t *init();
int add(table_t *table, char *data);
int del(table_t *table, char *data);
int find(table_t *table, char *data);
int show(table_t *table);
int flush(table_t *table);
#endif