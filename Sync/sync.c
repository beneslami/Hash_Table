#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include "sync.h"
#include "../linkedlist/linkedlist.h"

extern void *reader(void*);
int process_sync_msg(table_t *table, char *sync_msg, char *key){
	
	char data[32];
	void *ret_vpr;
	pthread_t tid;
	if(!strcmp(sync_msg, "UPDATE")){
	}

	else if(!strcmp(sync_msg, "ADD")){
		pack_t *pack = calloc(1, sizeof(pack));
		strcpy(pack->key, key);
		pthread_create(&tid, NULL, reader, (void*)pack);
		pthread_join(tid, &ret_vpr);
		add(table, pack->data);
		free(pack);	
	}

	else if(!strcmp(sync_msg, "DELETE")){
		pack_t *pack = calloc(1, sizeof(pack_t));
		strcpy(pack->key, key);
		pthread_create(&tid, NULL, reader, (void*)pack);
		pthread_join(tid, &ret_vpr);		
		int rc = del(table, pack->data);
		if (rc == 0){
			char temp[34];
        	sprintf(temp, "/%s", pack->key);
        	shm_unlink(temp);   
		}	
		else{
			printf("Operation not done\n");
			return -1;	
		}
	}

	else if(!strcmp(sync_msg, "FLUSH")){
		flush(table);
		table = init();
	}
	
	else if(!strcmp(sync_msg, "NONE")){

	}
	return 0;
}

void hash_function(char* data, char* hash){
	int i=0;
	for( ; data[i] != '\0'; i++){
		hash[i] = (data[i]+9);
	}
	hash[i] = '\0';
}