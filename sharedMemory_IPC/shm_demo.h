#ifndef __SHM_DEMO__
#define __SHM_DEMO__
int create_and_write_shared_memory(char *mmap_key, char* value, unsigned int size);
int read_from_shared_memory(char *mmap_key, char *buffer, unsigned int buff_size, unsigned int bytes_to_read);

#endif __SHM_DEMO__
