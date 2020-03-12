#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "sync/sync.h"
#include "linkedlist/linkedlist.h"

#define SOCKET_NAME "socket"
int data_socket;
int loop = 1; // indicates if server is still up and running
int disconnect = 1; // indicates to server that client wants to disconnect

void signal_handler(int signal_num) {
    if (signal_num == SIGINT) {
        exit(0);
    }
    else if (signal_num == SIGUSR1) {
        exit(0);
    }
}

int main(void){

    table_t *table;
    table = init(); // create table
    OPCODE op_code;
	struct sockaddr_un addr;// create socket

	data_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (data_socket == -1) {
        perror("socket");
        exit(1);
    }
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);
    if (connect(data_socket, (const struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        fprintf(stderr, "The server is down.\n");
        exit(1);
    }
    printf("connected\n"); // create socket
    pid_t pid = getpid();
    write(data_socket, &pid, sizeof(pid_t)); // send server client's process id
    signal(SIGINT, signal_handler);  //register signal handler
    signal(SIGUSR1, signal_handler);
    while(loop){
        if(read(data_socket, &loop, sizeof(int)) == -1){
            perror("read");
            break;
        }
        printf("server wants me to disconnect\n");
        printf("%d", loop);
        break;
        
    }
    close(data_socket);
	return 0;
}