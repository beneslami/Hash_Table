#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

table_t *init(){
	table_t *table = calloc(1, sizeof(table_t));
	table_entry_t *node = calloc(1, sizeof(table_entry_t));
	node->next = node;
	node->next = node;
	return table;
}

int add(table_t *table, char *data){
	table_entry_t *head = table->next;
	table_entry_t *node = calloc(1, sizeof(table_entry_t));
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
	table_entry_t *temp;
	while(head->next){
		if(!strcmp(head->data, entry->data) || !strcmp(head->hash, entry->hash)){
			temp = head->next;
			head->next = temp->next;
			free(temp);
			return 0;
		}
		head = head->next;
	}
	return -1;
}

table_entry_t *find(table_t *table, char *data){
	table_entry_t *head = table->next;
	while(head->next){
		if(!strcmp(head->data, data)){
			return head;
		}
		head = head->next;
	}
	return NULL;
}

int show(table_t *table){
	table_entry_t *head = table->next;
	if(head == NULL){
		printf("table is empty\n");
		return -1;
	}
	while(head->next){
		printf("|%s\t%s|\n", head->data, head->hash);
		head = head->next;
	}
	return 0;
}

int flush(table_t *table){
	table_entry_t *head = table->next;
	while(head->next){
		free(head);
		head = head->next;	
	}
	free(table);
	return 0;
}