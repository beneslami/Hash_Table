CC = gcc
CFLAGS = -Wall -Wextra
DEPSRC = DLL/dll.c data-List/data-list.c Hash-Table/hash-table.c Sync/sync.c

.PHONY: dll data-list hash-table sync shm_ip

default: dll data-list hash-table sync shm_ip
	$(CC) $(CFLAGS) client.c shm_ip.c $(DEPSRC) -o client
	$(CC) $(CFLAGS) server.c shm_ip.c $(DEPSRC) -o server

client: dll data-list hash-table sync shm_ip
	$(CC) $(CFLAGS) client.c shm_ip.c $(DEPSRC) -o client

server: dll data-list hash-table sync shm_ip
	$(CC) $(CFLAGS) server.c shm_ip.c $(DEPSRC) -o server

dll:
	$(CC) $(CFLAGS) -c DLL/dll.c -o DLL/dll.o

data-list: dll shm_ip
	$(CC) $(CFLAGS) -c data-List/data-list.c -o data-List/data-list.o

hash-table: dll
	$(CC) $(CFLAGS) -c Hash-Table/hash-table.c -o  Hash-Table/hash-table.o

sync: dll data-list hash-table shm_ip
	$(CC) $(CFLAGS) -c Sync/sync.c -o Sync/sync.o

shm_ip:
	$(CC) $(CFLAGS) -c shm_ip.c -o shm_ip.o

clean:
	rm -f client server DLL/dll.o data-List/data-list.o Hash-Table/hash-table.o shm_ip.o Sync/sync.o
