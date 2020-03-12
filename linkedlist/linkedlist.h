#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct table_entry_{
	char data[32];
	char hash[32];
	struct table_entry_ *next;
}table_entry_t;

typedef struct table_{
	table_entry_t *next;
}table_t;

table_t *init();
int add(table_t *table, char *data);
int del(table_t *table, table_entry_t *entry);
table_entry_t *find(table_t *table, char *data);
int show(table_t *table);
int flush(table_t *table);
#endif