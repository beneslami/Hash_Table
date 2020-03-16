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

struct shm_buffer {
    char data[32];
    char hash[32];
    sem_t sem; //semaphore to protect access
};
struct shm_buffer *shmp;

#define DATA_LEN sizeof(struct shm_buffer)

int writer(const char *key, const char *data, const char *hash) {

    sem_init(&shmp->sem, 1, 1);
    int shm_fd = shm_open(key, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (shm_fd == -1) {
        printf("Could not create shared memory\n");
        return -1;
    }

    if (ftruncate(shm_fd, sizeof(struct shm_buffer)) == -1) {
        printf("Error on ftruncate to allocate \n");
        return -1;
    }

    shmp =  mmap(NULL, sizeof(struct shm_buffer), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shmp == MAP_FAILED){
        printf("Mapping failed\n");
        return -1;
    }
    
    sem_wait(&shmp->sem);
    memcpy(shmp->data, data, strlen(data));
    memcpy(shmp->hash, hash, strlen(hash));
    sem_post(&shmp->sem);

    if (munmap(shmp, sizeof(struct shm_buffer)) == -1) {
        printf("Unmapping failed\n");
        return -1;
    }

    close(shm_fd);
    return 1;
}

int reader(const char *key, char *data, char *hash) {
    int shm_fd = shm_open(key, O_CREAT | O_RDONLY , 0666);
    if (shm_fd == -1) {
        printf("Could not open shared memory \n");
        return -1;
    }

    shmp = mmap(NULL, DATA_LEN, PROT_READ, MAP_SHARED, shm_fd, 0);
    if(shmp == MAP_FAILED){
        printf("Mapping failed\n");
        return -1;
    }
    sem_wait(&shmp->sem);
    memcpy(data, shmp->data, 32);
    memcpy(hash, shmp->hash, 32);
    sem_post(&shmp->sem);

    if (munmap(shmp, sizeof(struct shm_buffer)) == -1) {
        printf("Unmapping failed\n");
        return -1;
    }
    close(shm_fd);
    return 1;
}