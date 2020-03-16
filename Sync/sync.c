#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sync.h"
#include "../linkedlist/linkedlist.h"

extern int reader(const char *key, char *data, const char *hash);
char *shm_key = "/shm";

int process_sync_msg(table_t *table, char *sync_msg){
	
	char hash[32];
	char data[32];
	if(!strcmp(sync_msg, "UPDATE")){
		table_entry_t *head = table->next;

		while(1){
			reader(shm_key, data, hash);
			printf("%s\n", data);
			if(!strcmp(data, "")|| !strcmp(hash, "")) {
				break;
			}
			table_entry_t *node = calloc(1, sizeof(table_entry_t));
			add(table, data);
		}
	}

	else if(!strcmp(sync_msg, "ADD")){
		table_entry_t *entry = table->next;
		table_entry_t *node = calloc(1, sizeof(table_entry_t));
		while(entry->next){
			entry = entry->next;
		}
		reader(shm_key, node->data, node->hash);
		entry->next = node;
		node->next = NULL;
		printf("try to read from the shared memory to add the item to the hash table\n");
	}

	else if(!strcmp(sync_msg, "DELETE")){
		//read the item from shared memory 
		printf("try to read from the shared memory to delete the item from the hash table\n");	
	}

	else if(!strcmp(sync_msg, "FLUSH")){
		printf("try to flush the table\n");
	}
	
	else if(!strcmp(sync_msg, "NONE")){
		
	}
	return 0;
}