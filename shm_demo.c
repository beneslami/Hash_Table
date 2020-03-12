#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define DATA_LEN 64

int writer(const char *key, const char *data) {
    int size = strlen(data);
    int shm_fd = shm_open(key, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (shm_fd == -1) {
        printf("Could not create shared memory\n");
        return -1;
    }

    if (ftruncate(shm_fd, size) == -1) {
        printf("Error on ftruncate to allocate \n");
        return -1;
    }

    void *shm_reg =  mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_reg == MAP_FAILED){
        printf("Mapping failed\n");
        return -1;
    }

    memset(shm_reg, 0, size);
    memcpy(shm_reg, data, size);

    if (munmap(shm_reg, size) == -1) {
        printf("Unmapping failed\n");
        return -1;
    }

    close(shm_fd);
    return size;
}

int reader(const char *key, char *data) {
    int shm_fd = shm_open(key, O_CREAT | O_RDONLY , 0666);
    if (shm_fd == -1) {
        printf("Could not open shared memory \n");
        return -1;
    }

    void *shm_reg = mmap(NULL, DATA_LEN, PROT_READ, MAP_SHARED, shm_fd, 0);
    if(shm_reg == MAP_FAILED){
        printf("Mapping failed\n");
        return -1;
    }
    memcpy(data, shm_reg, DATA_LEN);
  
    if (munmap(shm_reg, DATA_LEN) == -1) {
        printf("Unmapping failed\n");
        return -1;
    }
    close(shm_fd);
    return strlen(data);
}