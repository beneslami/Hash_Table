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
	}

	else if(!strcmp(sync_msg, "ADD")){
		table_entry_t *entry = table->next;
		reader(shm_key, data, hash);
		add(table, data);
	}

	else if(!strcmp(sync_msg, "DELETE")){
		table_entry_t *entry = table->next;
		table_entry_t *node;
		reader(shm_key, data, hash);
		node = find(table, data);
		if(node){
			del(table, node);	
		}
		else{
			printf("not found\n");
		}
	}

	else if(!strcmp(sync_msg, "FLUSH")){
		flush(table);
	}
	
	else if(!strcmp(sync_msg, "NONE")){
		
	}
	return 0;
}