/*header file for double linked list */
#ifndef __DLLH__
#define __DLLH__

typedef struct dll_node_{
  char data[32];
  char hash[32];
  struct dll_node_ *left;
  struct dll_node_ *right;
} dll_node_t;

typedef struct dll_{
  dll_node_t *head;
}dll_t;

/*Public function decleration to create and return a new empty dll*/
dll_t *get_new_dll();

/*Public function decleration to add the application data to the dll*/
int add_data_to_dll(dll_t *dll, dll_node_t *app_data);

/*Public function decleration to delete the application data from the dll*/
void delete_data_from_dll(dll_t *dll, dll_node_t *node);

/*public function decleration to search specific item in the dll */
dll_node_t *dll_search_by_key(dll_t *dll, char *key);


#endif __DLLH__
