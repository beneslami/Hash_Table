#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

table_t *init(){
	table_t *table = calloc(1, sizeof(table_t));
	table_entry_t *node = calloc(1, sizeof(table_entry_t));
	node->next = NULL;
	table->next = node;
	return table;
}

int add(table_t *table, char *data){
	table_entry_t *head = table->next;
	table_entry_t *node = calloc(1, sizeof(table_entry_t));
	
	if(!head->next){
		strcpy(node->data, data);
    	strcpy(node->hash, data);
    	node->next = NULL;
    	head->next = node;
    	return 0;
	}
	while(head->next){
		head = head->next;
	}
	strcpy(node->data, data);
	strcpy(node->hash, data); // to be changed to the output of the hash function
	node->next = NULL;
	head->next = node;
	return 0;
}

int del(table_t *table, table_entry_t *entry){
	table_entry_t *head = table->next;
	table_entry_t *temp, *current, *previous;
	current = head;
  	previous = head;
	if(!head->next){
	    free(head);
	    return 0;
	}
  	while(current){
	    if(!strcmp(current->data, entry->data)){
	      previous->next = current->next;
	      free(current);
	    }
    	previous = current;
    	current = current->next;
  	}
  	return 0;
}

table_entry_t *find(table_t *table, char *data){
	table_entry_t *head = table->next;
	while(head){
		if(!strcmp(head->data, data)){
			return head;
		}
		else{
			head = head->next;
		}
	}
	return NULL;
}

int show(table_t *table){
	table_entry_t *head = table->next;
	if(head == NULL){
		printf("table is empty\n");
		return -1;
	}
	while(head){
		printf("|%s\t%s|\n", head->data, head->hash);
		head = head->next;
	}
	return 0;
}

int flush(table_t *table){
	table_entry_t *node = table->next;
	while(node){
		del(table, node);
		node = node->next;
	}
	free(table);
	table = init();
	return 0;
}