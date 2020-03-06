CC = gcc
CFLAGS = -Wall -Wextra
DEPSRC = Double-Linked-List-Library/dll.c sharedMemory_IPC/shm_demo.c

server: dll hash_utils shm_demo client
	$(CC) $(CFLAGS) server.c hash_utils.c $(DEPSRC) -o server

client: shm_demo
	$(CC) $(CFLAGS) client.c $(DEPSRC) -o client

dll:
	$(CC) $(CFLAGS) -c Double-Linked-List-Library/dll.c -o Double-Linked-List-Library/dll.o

hash_utils:
	$(CC) $(CFLAGS) -c hash_utils.c -o hash_utils.o

shm_demo:
	$(CC) $(CFLAGS) -c sharedMemory_IPC/shm_demo.c -o sharedMemory_IPC/shm_demo.o -lrt

clean:
	rm -rf hash_utils.o server.o server client.o shm_demo.o client Double-Linked-List-Library/dll.o sharedMemory_IPC/shm_demo.o 
