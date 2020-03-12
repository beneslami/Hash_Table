#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "sync/sync.h"
#include "linkedlist/linkedlist.h"

#define SOCKET_NAME "socket"
#define MAX_CLIENTS 32
#define OP_LEN 128

int monitored_fd_set[MAX_CLIENTS];
pid_t client_pid_set[MAX_CLIENTS];

void intitiaze_monitor_fd_and_client_pid_set(int size){
    int i = 0;
    for(; i < size; i++) {
        monitored_fd_set[i] = -1;
        client_pid_set[i] = -1;
    }
}

void add_to_monitored_fd_set(int skt_fd, int size){
    int i = 0;
    for(; i < size; i++){
        if(monitored_fd_set[i] != -1)
            continue;
        monitored_fd_set[i] = skt_fd;
        break;
    }
}

void add_to_client_pid_set(int pid, int size){
    int i = 0;
    for(; i < size; i++){
        if(client_pid_set[i] != -1)
            continue;
        client_pid_set[i] = pid;
        break;
    }
}

void remove_from_monitored_fd_set(int skt_fd, int size){
    int i = 0;
    for(; i < size; i++){
        if(monitored_fd_set[i] != skt_fd)
            continue;
        monitored_fd_set[i] = -1;
        break;
    }
}

void remove_from_client_pid_set(int pid, int size){
    int i = 0;
    for(; i < size; i++){
        if(monitored_fd_set[i] != pid)
            continue;
        client_pid_set[i] = -1;
        break;
    }
}

void refresh_fd_set(fd_set *fd_set_ptr, int size){
    FD_ZERO(fd_set_ptr);
    int i = 0;
    for(; i < size; i++){
        if(monitored_fd_set[i] != -1){
            FD_SET(monitored_fd_set[i], fd_set_ptr);
        }
    }
}

void flush_clients(int size) {
    int i;
    for(i = 0; i < size; i++) {
        int pid = client_pid_set[i];
        if (pid != -1) {
            kill(pid, SIGUSR1);
        }
    }
}

int get_max_fd(int size){
    int i = 0;
    int max = -1;
    for(; i < size; i++){
        if(monitored_fd_set[i] > max)
            max = monitored_fd_set[i];
    }
    return max;
}

void signal_handler(int signal_num){
    if(signal_num == SIGINT)
    {
        unlink(SOCKET_NAME);
        exit(0);
    }
}

int main(void){
	
    fd_set readfds;
    table_t *table;
    table = init(); // create hash table

    intitiaze_monitor_fd_and_client_pid_set(MAX_CLIENTS);
    add_to_monitored_fd_set(0, MAX_CLIENTS);

	int connection_socket, data_socket, ret; //crete socket
    OPCODE op_code;
	struct sockaddr_un name;
	unlink(SOCKET_NAME);
	connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if(connection_socket == -1){
		perror("socket");
		return -1;
	}
	memset(&name, 0, sizeof(struct sockaddr_un));
	name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);
    ret = bind(connection_socket, (const struct sockaddr *) &name, sizeof(struct sockaddr_un));
    if (ret == -1) {
        perror("bind");
        exit(1);
    }
    ret = listen(connection_socket, 20);
    if (ret == -1) {
        perror("listen");
        exit(1);
    }//crete socket
    add_to_monitored_fd_set(connection_socket, MAX_CLIENTS);

    signal(SIGINT, signal_handler);  //register signal handlers

    while(1){
        char op[OP_LEN];

        refresh_fd_set(&readfds, MAX_CLIENTS); /*Copy the entire monitored FDs to readfds*/
    	
        select(get_max_fd(MAX_CLIENTS) + 1, &readfds, NULL, NULL, NULL);  /* Wait for incoming connections. */
	
		/* New connection: send entire table states to newly connected client. */
        if(FD_ISSET(connection_socket, &readfds)){
            data_socket = accept(connection_socket, NULL, NULL);
            if (data_socket == -1) {
                perror("accept");
                exit(1);
            }
        
            pid_t pid;
            if (read(data_socket, &pid, sizeof(pid_t)) == -1) {
                perror("read");
                exit(1);
            }
            printf("%d\n", pid);
            add_to_monitored_fd_set(data_socket, MAX_CLIENTS);
            add_to_client_pid_set(pid, MAX_CLIENTS);

            // update new client 
        }
        else if(FD_ISSET(0, &readfds)){
            ret = read(0, op, OP_LEN - 1);

            op[strcspn(op, "\r\n")] = 0; 
            if (ret == -1) {
                perror("read");
                return 1;
            }
            op[ret] = 0;

            // create sync message
        }
        else{
            int i;
            for(i = 2; i < MAX_CLIENTS; i++){
                if(FD_ISSET(monitored_fd_set[i], &readfds)){
                    int done;
                    int comm_socket_fd = monitored_fd_set[i];

                    ret = read(comm_socket_fd, &done, sizeof(int));
                    if (done == 1) { // this client is disconnecting
                        close(comm_socket_fd);
                        remove_from_monitored_fd_set(comm_socket_fd, MAX_CLIENTS);
                    }
                    else if (ret == -1) {
                        perror("read");
                        exit(1);
                    }
                    else {
                        printf("%i\n", done);
                    }
                }
            }
        }
    }
	close(connection_socket);
	return 0;
}