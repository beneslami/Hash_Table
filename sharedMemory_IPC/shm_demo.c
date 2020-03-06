#include <stdio.h>
#include <memory.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

int create_and_write_shared_memory(char *mmap_key, char* value, unsigned int size){
  int shm_fd;
  shm_fd = shm_open(mmap_key, O_CREAT | O_RDWR | O_TRUNC, 0777);
  if(shm_fd < 0){
    printf("failure in opening or creating shared memory, errcode = %d\n", errno);
    return -1;
  }
  if(ftruncate(shm_fd, size) == -1){
    printf("error on truncate to allocate size of the shared memory region\n");
    return -1;
  }

  /* NULL-> let the OS choose where to allocate shared memory space*/
  void *shm_reg = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  /* shm_reg-> the address in the process virtual address space.
     The linux paging mechanism maps this address to starting address of the shared memory region in kernel
     space.*/
  memset(shm_reg, 0, size);
  memcpy(shm_reg, value, size);
  munmap(shm_reg, size);
  close(shm_fd);
  return size;
}

int read_from_shared_memory(char *mmap_key, char *buffer, unsigned int buff_size, unsigned int bytes_to_read){
  int shm_fd = 0 ;
  int rc = 0;

  shm_fd = shm_open(mmap_key, O_CREAT |O_RDONLY, 0777);
  if(shm_fd < 0){
    printf("failure in opening shared memory, errcode = %d\n", errno);
    return -1;
  }
  void *shm_reg = mmap(NULL, bytes_to_read, PROT_READ, MAP_SHARED, shm_fd, 0);
  if(shm_reg == MAP_FAILED){
    printf("Error in mapping\n");
    return -1;
  }
  memcpy(buffer, shm_reg, bytes_to_read);
  rc = munmap(shm_reg, bytes_to_read);
  if(rc < 0){
    printf("failure on unmap\n");
    return -1;
  }
  close(shm_fd);
  return bytes_to_read;
}
