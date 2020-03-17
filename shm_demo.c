#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "linkedlist/linkedlist.h"

#define DATA_LEN 70

int writer(const char *key, const char *data, const char *hash) {

    int shm_fd = shm_open(key, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        printf("Could not create shared memory\n");
        return -1;
    }

    if (ftruncate(shm_fd, DATA_LEN) == -1) {
        printf("Error on ftruncate to allocate \n");
        return -1;
    }

    void *shmp_wr =  mmap(NULL, DATA_LEN, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shmp_wr == MAP_FAILED){
        printf("Mapping failed\n");
        return -1;
    }
    char shm_data[70];
    sprintf(shm_data, "%s->%s", data, hash);
    memcpy(shmp_wr, shm_data, strlen(shm_data));
   

    if (munmap(shmp_wr, DATA_LEN) == -1) {
        printf("Unmapping failed\n");
        return -1;
    }   
    close(shm_fd);
    return 1;
}

int reader(const char *key, char *data, char *hash) {
    int shm_fd = shm_open(key, O_CREAT | O_RDONLY , S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        printf("Could not open shared memory \n");
        return -1;
    }

    void *shmp_rd = mmap(NULL, DATA_LEN, PROT_READ, MAP_SHARED, shm_fd, 0);
    if(shmp_rd == MAP_FAILED){
        printf("Mapping failed\n");
        return -1;
    }
    
    char shm_data[70];
    memcpy(shm_data, shmp_rd, DATA_LEN);
    sscanf(shm_data, "%s->%s", data, hash);

    if (munmap(shmp_rd, DATA_LEN) == -1) {
        printf("Unmapping failed\n");
        return -1;
    }
    shm_unlink(key);
    close(shm_fd);
    return 1;
}