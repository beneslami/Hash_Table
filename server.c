#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Double-Linked-List-Library/dll.h"
#include "sharedMemory_IPC/shm_demo.h"

extern void hashfunc(char* buf, int len);
extern int read_from_shared_memory(char *mmap_key, char *buffer, unsigned int buff_size, unsigned int bytes_to_read);


int main(int argc, char **argv){
  dll_t *table;
  dll_node_t *item, *head;
  char *temp;
  int select, rc;
  char *shm_key ="/test";
  char read_buffer[128];
  memset(read_buffer, 0, 128);

  table = get_new_dll();

  rc = read_from_shared_memory(shm_key, read_buffer, 128, 128);
  if(rc < 0){
    printf("error reading from shared memory\n");
    return 0;
  }
  int op_code;
  char ptr[32];
  sscanf(read_buffer, "%d%s", &op_code, ptr);

  if (op_code == 1){ //add item to the table
    int i;
    sscanf(read_buffer, "%d%s", &i, ptr);
    item = calloc(1, sizeof(dll_node_t));
    strcpy(item->data, ptr);
    hashfunc(ptr, strlen(ptr));
    strcpy(item->hash, ptr);
    add_data_to_dll(table, item);
    free(item);
    memset(read_buffer, 0, 128);
  }
  else if(op_code == 2){//delete item from the table
    int i;
    sscanf(read_buffer, "%d%s", &i, ptr);
    item = calloc(1, sizeof(dll_node_t));
    item = dll_search_by_key(table, ptr);
    delete_data_from_dll(table, item);
    free(item);
    memset(read_buffer, 0, 128);
  }
  else if(op_code == 3){ //show items in the table
    head = table->head;
    while(head){
      printf("%s\t%s\n", head->data, head->hash);
      head = head->right;
    }
    memset(read_buffer, 0, 128);
  }

  shm_unlink(shm_key);
  free(table);
  return 0;
}
