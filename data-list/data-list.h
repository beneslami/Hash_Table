#ifndef DATA_LIST_H
#define DATA_LIST_H

#define DATA_LEN 18

typedef struct dll_ dll_t;
typedef struct dll_node_ dll_node_t;

/* Data structure definition for data list entry */
typedef struct data_list_entry_ {
    char data[DATA_LEN];
} data_list_entry_t;

/* Public API's for data list functionality */
void display_data_list(const dll_t *data_list);
dll_node_t *find_data(const dll_t *data_list, const char *data);

#endif
