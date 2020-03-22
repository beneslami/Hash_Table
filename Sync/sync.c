#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "sync.h"
#include "../linkedlist/linkedlist.h"

extern void *reader(void*);

int process_sync_msg(table_t *table, char *sync_msg){
	
	char hash[32];
	char data[32];
	void *ret_vpr;
	pthread_t tid;
	if(!strcmp(sync_msg, "UPDATE")){
	}

	else if(!strcmp(sync_msg, "ADD")){
		pthread_create(&tid, NULL, reader, NULL);
		pthread_join(tid, &ret_vpr);
		pack_t *ret = (pack_t *)ret_vpr;
		add(table, ret->data);
	}

	else if(!strcmp(sync_msg, "DELETE")){
		table_entry_t *node;
		int i = pthread_create(&tid, NULL, reader, NULL);
		i = pthread_join(tid, &ret_vpr);
		pack_t *ret = (pack_t *)ret_vpr;
		node = find(table, ret->data);
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