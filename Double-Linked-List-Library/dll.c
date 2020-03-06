#include <memory.h>
#include <stdlib.h>
#include "dll.h"

dll_t *get_new_dll(){
  dll_t *dll = calloc(1, sizeof(dll_t));
  dll-> head = NULL;
  return dll;
}

int add_data_to_dll(dll_t *dll, dll_node_t *item){
  if( !dll || !item)
    return -1;

  dll_node_t *dll_new_node = calloc(1, sizeof(dll_node_t));
  dll_new_node->left = NULL;
  dll_new_node->right = NULL;
  dll_new_node = item;

  /*add the data to the end of dll*/
  if(!dll->head){
    dll->head = dll_new_node;
    return 0;
  }

  dll_node_t *first_node = dll->head;
  dll_new_node->right = first_node;
  first_node->left = dll_new_node;
  dll->head = dll_new_node;
  return 0;
}

void delete_data_from_dll(dll_t *dll, dll_node_t *node) {
  node->left->right = node->right;
  node->right->left = node->left;
  free(node);
}

dll_node_t *dll_search_by_key(dll_t *dll, char *key){
  if(!dll || !dll->head)
    return NULL;
  dll_node_t *head = dll->head;
  while(head){
    if(strcmp(head->data, key) == 0)
      return head;
    head = head->right;
  }
  return NULL;
}
