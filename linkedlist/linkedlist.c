#include "linkedlist.h"
#include "../sync/sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/mman.h>

table_t *init(){
    table_t *table = calloc(1, sizeof(table_t));
    table_entry_t *node = calloc(1, sizeof(table_entry_t));
    node->next_hash = NULL;
    node->next_data = NULL;
    strcpy(node->hash, "NULL"); 
    table->next = node;
    return table;
}

int add(table_t *table, char *data){
    table_entry_t *head = table->next;
    table_entry_t *prev;
    char hash[32];
    hash_function(data, hash);

    if(!strcmp(head->hash, "NULL")){ 
        data_entry_t *item = calloc(1, sizeof(data_entry_t));
        strcpy(item->data, data);
        item->next = NULL;
        strcpy(head->hash, hash);
        head->next_data = item;
        head->next_hash = NULL;
        return 0;
    }
    while(head){
        if(!strcmp(head->hash, hash)){
            data_entry_t *temp = head->next_data;
            data_entry_t *previous;
            while(temp){
                if(!strcmp(temp->data, data)){
                    printf("data exists\n");
                    return 0;
                }
                previous = temp;
                temp = temp->next;
            }
            data_entry_t *item = calloc(1, sizeof(data_entry_t));
            strcpy(item->data, data);
            item->next = NULL;
            previous->next = item;
            return 0;
        }
        prev = head;
        head = head->next_hash;
    }

    table_entry_t *pack = calloc(1, sizeof(table_entry_t));
    data_entry_t *item = calloc(1, sizeof(data_entry_t));
    strcpy(pack->hash, hash);
    strcpy(item->data, data);
    item->next = NULL;
    pack->next_data = item;
    prev->next_hash = pack;
    return 0;
}

int del(table_t *table, char* data){
    table_entry_t *head = table->next;
    char hash[32];
    hash_function(data, hash);
    data_entry_t *previous;
    table_entry_t *prev;

    while(head){
        if(!strcmp(head->hash, hash)){
            data_entry_t *current = head->next_data;
            
            while(current){
                if(!strcmp(current->data, data)){
                    if(current == head->next_data){
                        head->next_data = current->next;
                        if(head->next_data == NULL){
                            table->next = head->next_hash;
                            free(head);
                        }
                        return 0;
                    }
                    else if (current->next == NULL){
                        free(current);
                        if(head->next_data == NULL){
                            prev->next_hash = head->next_hash;
                            free(head);
                        }
                        return 0;
                    }
                    else{
                        previous->next = current->next;
                        free(current);
                        if(head->next_data == NULL){
                            prev->next_hash = head->next_hash;
                            free(head);
                        }
                        return 0;
                    }
                }
                previous = current;
                current = current->next;
            }
        }
        prev = head;
        head = head->next_hash;
    }
    printf("Data does not exist\n");
    return -1;
}

int find(table_t *table, char *data){
    table_entry_t *head = table->next;
    data_entry_t *current;
    char hash[32];
    hash_function(data, hash);

    while(head){
        if(!strcmp(head->hash, hash)){
            current = head->next_data;
            while(!strcmp(current->data, data)){
                printf("%s\n", current->data);
                return 0;
            }
            current = current->next;
        }
        head = head->next_hash;
    }
    printf("Data does not exist\n");
    return -1;
}

int show(table_t *table){
    table_entry_t *head = table->next;
    data_entry_t *current; 
    if(head->next_data == NULL){
        printf("table is empty\n");
        return -1;
    }
    while(head){
        printf("%s \u2192 ",head->hash);
        current = head->next_data;
            while(current){
                printf("%s \u2192 ", current->data);
                current = current->next;
            }
        printf(" NULL\n");
        head = head->next_hash;
        if(head){
            printf("\u2193\n");
        }
    }
    return 0;
}

int flush(table_t *table){
    table_entry_t *head = table->next;
    table_entry_t *temp;
    data_entry_t *current, *previous; 
    if(head->next_data == NULL){
        printf("table is empty\n");
        return -1;
    }
    strcpy(head->hash, "NULL");
    while(head){
        current = head->next_data;
        while(current){
            previous = current;
            current = current->next;
            free(previous);
        }
        temp = head;
        head = head->next_hash;
        free(temp);
    }  
    free(table);
    return 0;
}