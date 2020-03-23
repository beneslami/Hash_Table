#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include "linkedlist/linkedlist.h"
#include "sync/sync.h"

#define DATA_LEN 70
pthread_mutex_t mutex;

void synchronizer_init(){
    pthread_mutex_init(&mutex, NULL);
}

void* writer(void* arg) {
    pack_t *pack;
    pack = (pack_t*) arg;
    char key[34];
    sprintf(key, "/%s", pack->key);
    printf("%s\n", key);
    pthread_mutex_lock(&mutex);
    int shm_fd = shm_open(key, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        int errnum;
        printf("Could not create shared memory\n");
        errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("error");
        fprintf(stderr, "Error opening file: %s\n", strerror( errno ));
        return (void*)-1;
    }

    if (ftruncate(shm_fd, DATA_LEN) == -1) {
        printf("Error on ftruncate to allocate \n");
        return (void*)-1;
    }

    void *shmp_wr =  mmap(NULL, DATA_LEN, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shmp_wr == MAP_FAILED){
        printf("Mapping failed\n");
        return (void*)-1;
    }
    char shm_data[70];
    sprintf(shm_data, "%s -> %s", pack->data, pack->hash);
    memcpy(shmp_wr, shm_data, strlen(shm_data));
   
    if (munmap(shmp_wr, DATA_LEN) == -1) {
        printf("Unmapping failed\n");
        return (void*)-1;
    }   
    close(shm_fd);
    pthread_mutex_unlock(&mutex);
    return (void*)1;
}

void *reader(void* arg) {
    pack_t *pack = calloc(1, sizeof(pack_t)); 
    char data[32];
    char hash[32];
    char key[32];
    sprintf(key, "/%s",(char*)arg);
    printf("%s\n", key);
    pthread_mutex_lock(&mutex);
    int shm_fd = shm_open(key, O_CREAT | O_RDONLY , S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        printf("Could not open shared memory \n");
        return (void*)-1;
    }

    void *shmp_rd = mmap(NULL, DATA_LEN, PROT_READ, MAP_SHARED, shm_fd, 0);
    if(shmp_rd == MAP_FAILED){
        printf("Mapping failed\n");
        return (void*)-1;
    }
    
    char shm_data[70];
    memcpy(shm_data, shmp_rd, DATA_LEN);
    sscanf(shm_data, "%s -> %s", data, hash);

    if (munmap(shmp_rd, DATA_LEN) == -1) {
        printf("Unmapping failed\n");
        return (void*)-1;
    }
    strcpy(pack->data, data);
    strcpy(pack->hash, hash);
    close(shm_fd);
    pthread_mutex_unlock(&mutex);
    
    return (void*)pack;
}