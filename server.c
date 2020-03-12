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

int create_sync_message(char*, char*);
int monitored_fd_set[MAX_CLIENTS];
pid_t client_pid_set[MAX_CLIENTS];
table_t *table;


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

int create_sync_message(char *operation, char *sync_msg){
    char code[6], data[32];
    sscanf(operation, "%s %s", code, data);
    OPCODE op_code;
    if(!strcmp(code, "ADD"))
        op_code = ADD;
    else if(!strcmp(code, "DELETE"))
        op_code = DELETE;
    else if(!strcmp(code, "FIND"))
        op_code = FIND;
    else if(!strcmp(code, "SHOW"))
        op_code = SHOW;
    else if(!strcmp(code, "FLUSH"))
        op_code = FLUSH;
    else{
        printf("invalid opcode\n");
        return -1;
    }
    switch(op_code){
        case ADD:
            strcpy(sync_msg, "ADD");
            //insert data to the shared memory
        break;
        case DELETE:
            strcpy(sync_msg, "DELETE");
            //insert data to the shared memory
        break;
        case FIND:
            strcpy(sync_msg, "FIND");
        break;
        case SHOW:
            strcpy(sync_msg, "NONE");
            show(table);
        break;
        case FLUSH:
            strcpy(sync_msg, "FLUSH");
            flush(table);
        break;
        default:
        break;
    }
    return 0;
}

int main(void){
	
    char loop = '1';
    char sync_msg[6];
    fd_set readfds;
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
    	printf("Enter operation\n");
        printf("1) ADD <DATA>\n");
        printf("2) DELETE <DATA>\n");
        printf("3) FIND <DATA>\n");
        printf("4) SHOW\n");
        printf("5) FLUSH\n");

        select(get_max_fd(MAX_CLIENTS) + 1, &readfds, NULL, NULL, NULL);  /* Wait for incoming connections. */
	
		
        if(FD_ISSET(connection_socket, &readfds)){    /* New connection */
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
            
            add_to_monitored_fd_set(data_socket, MAX_CLIENTS);
            add_to_client_pid_set(pid, MAX_CLIENTS);

            // update new client
            //update_new_client(); 
        }
        else if(FD_ISSET(0, &readfds)){  /* read from console */
            ret = read(0, op, OP_LEN - 1);
            op[ret] = '\0';

            if(ret < 0){
                printf("Insert valid operation\n");
                break;
            }
            create_sync_message(op, sync_msg);
            
            int i, comm_socket_fd;
            for (i = 2; i < MAX_CLIENTS; i++) { // start at 2 since 0 for server's console and 1 for connection_socket
                comm_socket_fd = monitored_fd_set[i];
                if (comm_socket_fd != -1) {
                    write(comm_socket_fd, sync_msg, sizeof(sync_msg));
                    //write(comm_socket_fd, &loop, sizeof(char));
                }
            }
            system("clear");
        }
        else{      /* Notify existing clients of changes */
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