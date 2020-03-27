CC = gcc
DEPSRC = sync/sync.c linkedlist/linkedlist.c shm_demo.c benchmark.c
CFLAGS = -Wall -Wextra

TARGET: server client

client: client.c $(DEPSRC)
	$(CC) client.c $(DEPSRC) -o client -lpthread

server: server.c $(DEPSRC)
	$(CC) server.c $(DEPSRC) -o server -lpthread

#shm_demo.o: shm_demo.c
#	$(CC) shm_demo.c 

clean:
	rm -rf client server socket