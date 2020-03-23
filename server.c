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

extern void synchronizer_init();
extern void *writer(void*);
int create_sync_message(char*, char*, char*);
void update_new_client(int, char*);

int monitored_fd_set[MAX_CLIENTS];
pid_t client_pid_set[MAX_CLIENTS];
table_t *table;
char loop = '1';


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

int create_sync_message(char *operation, char *sync_msg, char *key){
    char code[7], data[32];
    sscanf(operation, "%s %s", code, data);
    OPCODE op_code;
    
    if(!strcmp(code, "ADD")){
        op_code = ADD;
        strcpy(sync_msg, "ADD");
        strcpy(key, data);
        add(table, data);
        return 0;
    }

    else if(!strcmp(code, "DELETE")){
        op_code = DELETE;
        strcpy(sync_msg, "DELETE");
        strcpy(key, data);
        table_entry_t *node = find(table, data);
        del(table, node);
        return 0;
    }
    else if(!strcmp(code, "FIND")){
        op_code = FIND;
        strcpy(sync_msg, "FIND");
        table_entry_t *node = find(table, data);
        if(node){
            printf("%s\n", node->hash);
        }
        return -1;
    }

    else if(!strcmp(code, "SHOW")){
        op_code = SHOW;
        strcpy(sync_msg, "NONE");
        show(table);
        return 0;
    }

    else if(!strcmp(code, "FLUSH")){
        op_code = FLUSH;
        strcpy(sync_msg, "FLUSH");
        flush(table);
        return 0;
    }

    else if(!strcmp(code, "UPDATE")){
        op_code = UPDATE;
        
        return 0;
    }

    else{
        printf("invalid opcode\n");
        return -1;
    }
}

void update_new_client(int data_socket, char *sync_msg){     
    
    strcpy(sync_msg, "ADD");
    char op[40];
    table_entry_t *node = table->next->next;

    pthread_t tid;
    if(node){
        while(node){
            void* ret_vpr;
            pack_t *pack = calloc(1, sizeof(pack_t));
            
            strcpy(pack->data, node->data);
            strcpy(pack->hash, node->hash);
            strcpy(pack->key, node->data);
            sprintf(op, "%c %s %s", loop, sync_msg, node->data); 
            /* loop: indicate that the server keeps connection with client, 
               sync_msg: indicate the op code,
               node->data: indicate key for shared memory 
            */
            pthread_create(&tid, NULL, writer, (void *)pack);
            pthread_join(tid, &ret_vpr);
            int i = (int)ret_vpr;
            printf("%d\n", i);
            if(i == -1){
                printf("error in shared memory\n");
                break;
            }
            sleep(1);
            write(data_socket, op, sizeof(op));
            sleep(1);
            node = node->next;
            free(pack);
        }
    }
    else{
        printf("list is empty\n");
    }
}

int main(void){
	//unlink("/shm");
    //unlink("/shm1");
    char sync_msg[7];
    fd_set readfds;
    table = init(); // create hash table
    synchronizer_init();
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

            update_new_client(data_socket, sync_msg);
        }
        else if(FD_ISSET(0, &readfds)){               /* read from console */
            ret = read(0, op, OP_LEN - 1);
            op[strcspn(op, "\r\n")] = 0; // flush new line
            if(ret < 0){
                printf("Insert valid operation\n");
                break;
            }
            op[ret] = 0;
            char temp[45];
            char key[32];

            if(!create_sync_message(op, sync_msg, key)){
                int i, comm_socket_fd;
                for (i = 2; i < MAX_CLIENTS; i++) { // start at 2 since 0 for server's console and 1 for connection_socket
                    comm_socket_fd = monitored_fd_set[i];
                    if (comm_socket_fd != -1) {
                        sprintf(temp, "%c %s %s", loop, sync_msg, key);
                        write(comm_socket_fd, temp, sizeof(temp));
                        sleep(1);
                    }
                }
            }
            system("clear");
            fflush(stdin);
        }
        else{                                         /* Notify existing clients of changes */
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