CC = gcc
DEPSRC = sync/sync.c linkedlist/linkedlist.c shm_demo.c
CFLAGS = -Wall -Wextra

TARGET: server client

client: client.c $(DEPSRC)
	$(CC) client.c $(DEPSRC) -o client

server: server.c $(DEPSRC)
	$(CC) server.c $(DEPSRC) -o server

#shm_demo.o: shm_demo.c
#	$(CC) shm_demo.c 

clean:
	rm -rf client server socket