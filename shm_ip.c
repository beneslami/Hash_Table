#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "Hash-Table/hash-table.h"

/* Stores hash in newly created shared memory region corresponding to its corresponding data.
Returns the size of the created shm on success otherwise -1 on failure. */
int store_hash(const char *data, const char *hash) {
    size_t size = strlen(hash); // account for terminating null byte
    int shm_fd = shm_open(data, O_CREAT | O_RDWR | O_TRUNC, 0660);
    if (shm_fd == -1) {
        printf("Could not create shared memory for data %s - hash %s pair\n", data, hash);
        return -1;
    }

    if (ftruncate(shm_fd, size) == -1) {
        printf("Error on ftruncate to allocate size for hash %s\n", hash);
        return -1;
    }

    void *shm_reg =  mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_reg == MAP_FAILED){
        printf("Mapping failed\n");
        return -1;
    }

    memset(shm_reg, 0, size);
    memcpy(shm_reg, hash, size);

    if (munmap(shm_reg, size) == -1) {
        printf("Unmapping failed\n");
        return -1;
    }

    close(shm_fd);
    return size;
}

/* Get the hash corresponding to the data from the shared region that the server created.
Returns number of bytes on succcess else -1 on failure. */
int get_hash(const char *data, char *hash) {
    int shm_fd = shm_open(data, O_CREAT | O_RDONLY , 0660);
    if (shm_fd == -1) {
        printf("Could not open shared memory for data %s - hash %s pair\n", data, hash);
        return -1;
    }

    void *shm_reg = mmap(NULL, HASH_LEN, PROT_READ, MAP_SHARED, shm_fd, 0);
    if(shm_reg == MAP_FAILED){
        printf("Mapping failed\n");
        return -1;
    }

    memcpy(hash, shm_reg, HASH_LEN);
    if (munmap(shm_reg, HASH_LEN) == -1) {
        printf("Unmapping failed\n");
        return -1;
    }

    close(shm_fd);
    return strlen(hash);
}

void hashfunction(char *data, int size){
  int i = 0;
  for (; i< size/2; i++){
      if(i%3 == 0){
        data[i] = 'Z';
      }
      else if(i%3 == 1){
        data[i] = 'Q';
      }
      else if(i%3 == 2){
        data[i] = 'E';
      }
  }
}
