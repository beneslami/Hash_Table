#include <stdio.h>
#include <string.h>
#include "sync.h"
#include "../linkedlist/linkedlist.h"

int process_sync_msg(table_t *table, char *sync_msg){
	if(!strcmp(sync_msg, "UPDATE")){
		printf("try to read from the shared memory to create carbon copy of hash table\n");
	}
	else if(!strcmp(sync_msg, "ADD")){
		printf("try to read from the shared memory to add the item to the hash table\n");
	}
	else if(!strcmp(sync_msg, "DELETE")){
		printf("try to read from the shared memory to delete the item from the hash table\n");	
	}
	else if(!strcmp(sync_msg, "FLUSH")){
		printf("try to flush the table\n");
	}
	else if(!strcmp(sync_msg, "NONE")){
		
	}
	return 0;
}