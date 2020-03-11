#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <semaphore.h>
#include "DLL/dll.h"
#include "data-List/data-list.h"
#include "Hash-Table/hash-table.h"
#include "Sync/sync.h"
#define SEM_MUTEX_NAME "/sem-mutex"
#define MAX_CLIENTS 32
#define OP_LEN 128

extern int store_hash(const char *data, const char *hash);
extern void hashfunction(char *data, int size);

int synchronized; // indicates if server has finished sending current updates
int connection_socket; // socket to establish connections with new clients
int loop = 1; // indicates if server is still running to clients

/* Server's copies of network data structures */
dll_t *hash_table;
dll_t *data_list;

/*An array of File descriptors which the server process is maintaining in order to talk with the connected clients. Master skt FD is also a member of this array*/
int monitored_fd_set[MAX_CLIENTS];
pid_t client_pid_set[MAX_CLIENTS]; // array of client process id's

/*Remove all the FDs and client pid's, if any, from the the array*/
void intitiaze_monitor_fd_and_client_pid_set(){
    int i = 0;
    for(; i < MAX_CLIENTS; i++) {
        monitored_fd_set[i] = -1;
        client_pid_set[i] = -1;
    }
}

/*Add a new FD to the monitored_fd_set array*/
void add_to_monitored_fd_set(int skt_fd){
    int i = 0;
    for(; i < MAX_CLIENTS; i++){
        if(monitored_fd_set[i] != -1)
            continue;
        monitored_fd_set[i] = skt_fd;
        break;
    }
}

/*Add a new pid to the client_pid_set array*/
void add_to_client_pid_set(int pid){
    int i = 0;
    for(; i < MAX_CLIENTS; i++){
        if(client_pid_set[i] != -1)
            continue;
        client_pid_set[i] = pid;
        break;
    }
}

/*Remove the FD from monitored_fd_set array*/
void remove_from_monitored_fd_set(int skt_fd){
    int i = 0;
    for(; i < MAX_CLIENTS; i++){
        if(monitored_fd_set[i] != skt_fd)
            continue;
        monitored_fd_set[i] = -1;
        break;
    }
}

/*Remove the pid from client_pid_set array*/
void remove_from_client_pid_set(int pid){
    int i = 0;
    for(; i < MAX_CLIENTS; i++){
        if(monitored_fd_set[i] != pid)
            continue;
        client_pid_set[i] = -1;
        break;
    }
}

/* Clone all the FDs in monitored_fd_set array into fd_set Data structure*/
void refresh_fd_set(fd_set *fd_set_ptr){
    FD_ZERO(fd_set_ptr);
    int i = 0;
    for(; i < MAX_CLIENTS; i++){
        if(monitored_fd_set[i] != -1){
            FD_SET(monitored_fd_set[i], fd_set_ptr);
        }
    }
}

/* Inform clients to flush their hash tables and data lists*/
void flush_clients() {
    int i;
    for(i = 0; i < MAX_CLIENTS; i++) {
        int pid = client_pid_set[i];
        if (pid != -1) {
            kill(pid, SIGUSR1);
        }
    }
}

/*Get the numerical max value among all FDs which server is monitoring*/
int get_max_fd(){
    int i = 0;
    int max = -1;
    for(; i < MAX_CLIENTS; i++){
        if(monitored_fd_set[i] > max)
            max = monitored_fd_set[i];
    }
    return max;
}

/* Parses a string command, in the format <Opcode, Data> with
each field separated by a space, to create a sync message for clients, instructing
them on how to update their copies of the hash table. The silent parameter indicates
whether the server is actively inputting a command for data list via stdin or a client
is replicating a command sent by the server. Returns 0 on success and -1 on any failure. */

int create_sync_message(char *operation, sync_msg_t *sync_msg, int silent) {
    char *token = strtok(operation, " ");
    if (token) {
        switch (token[0]) {
            case 'C':
                sync_msg->op_code = CREATE;
                break;
            case 'D':
                sync_msg->op_code = DELETE;
                break;
            case 'S':
                sync_msg->op_code = NONE;
                display_hash_table(hash_table);
                display_data_list(data_list);
                return 0;
            case 'F':
                sync_msg->op_code = NONE;
                flush_clients();
                deinit_dll(hash_table);
                deinit_dll(data_list);
                hash_table = init_dll();
                data_list = init_dll();
                return 0;
            default:
                fprintf(stderr, "Invalid operation: unknown op code\n");
                return -1;
        }
    }
    else {
        fprintf(stderr, "Invalid operation: missing op code\n");
        return -1;
    }
    token = strtok(NULL, " ");
    sync_msg->l_code = DATA_L;
    if(token){
      memcpy(sync_msg->msg_body.data_list_entry.data, token, strlen(token));
    }
    else{
      printf("Damn! something is wrong\n");
    }

    return 0;
}

/* Break out of main infinite loop and inform clients of shutdown to exit cleanly. */
void signal_handler(int signal_num){
    if(signal_num == SIGINT)
    {
        int i, synchronized = WAIT, loop = 0;
        sync_msg_t sync_msg;
        sync_msg.op_code = NONE;
        for(i = 2; i < MAX_CLIENTS; i++){
            int comm_socket_fd = monitored_fd_set[i];
            if (comm_socket_fd != -1) {
                write(comm_socket_fd, &sync_msg, sizeof(sync_msg_t));
                write(comm_socket_fd, &synchronized, sizeof(int));
                write(comm_socket_fd, &loop, sizeof(int));
            }
        }
        /* Clean up resources */
        deinit_dll(hash_table);
        deinit_dll(data_list);
        close(connection_socket);
        remove_from_monitored_fd_set(connection_socket);
        unlink(SOCKET_NAME);
        exit(0);
    }
}

/* Send newly client all necessary CREATE commands to replicate the server's copies of the
current hash table or data list. */
void update_new_client(int data_socket, LCODE l_code, char *op, sync_msg_t *sync_msg) {
    dll_node_t *head = l_code == HASH_L ? hash_table->head : data_list->head;
    dll_node_t *curr = head->next;

    while (curr != head) {
        hash_table_entry_t ht_entry = *((hash_table_entry_t *) curr->data);
        data_list_entry_t dl_entry = *((data_list_entry_t *) curr->data);

        sync_msg->op_code = CREATE;
        if (l_code == HASH_L) {
            sprintf(op, "C %s", ht_entry.hash);
        }
        else {
            sprintf(op, "C %s", dl_entry.data);
        }
        create_sync_message(op, sync_msg, 1);

        write(data_socket, sync_msg, sizeof(sync_msg_t));
        write(data_socket, &synchronized, sizeof(int));
        write(data_socket, &loop, sizeof(int));

        curr = curr->next;
    }

    /* Send dummy sync message to inform client that all current updates have been sent. */
    sync_msg->op_code = NONE;
    write(data_socket, sync_msg, sizeof(sync_msg_t));
    synchronized = l_code == HASH_L ? HASH_LIST : DATA_LIST;
    write(data_socket, &synchronized, sizeof(int));
    write(data_socket, &loop, sizeof(int));
}


int main() {
    struct sockaddr_un name;
    int ret;
    int data_socket;
    fd_set readfds;

    hash_table = init_dll();
    data_list = init_dll();
    sem_init(sem_access, 1, 1);

    intitiaze_monitor_fd_and_client_pid_set();
    add_to_monitored_fd_set(0);

    unlink(SOCKET_NAME); //In case the program exited inadvertently on the last run, remove the socket.

    /* master socket for accepting connections from client */
    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connection_socket == -1) {
        perror("socket");
        exit(1);
    }

    /*initialize*/
    memset(&name, 0, sizeof(struct sockaddr_un));

    /*Specify the socket Cridentials*/
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

    /* Bind socket to socket name.*/
    ret = bind(connection_socket, (const struct sockaddr *) &name,
               sizeof(struct sockaddr_un));
    if (ret == -1) {
        perror("bind");
        exit(1);
    }

    /* Prepare for accepting connections.  */
    ret = listen(connection_socket, 20);
    if (ret == -1) {
        perror("listen");
        exit(1);
    }

    add_to_monitored_fd_set(connection_socket);

    signal(SIGINT, signal_handler);  //register signal handlers

    /* The server continuously checks for new client connections, monitors existing connections
    (i.e. incoming messages and inactive connections, modifies the hash table or data list if need be,
    and broadcasts any changes to clients. */
    while (1) {
        char op[OP_LEN];
        sync_msg_t *sync_msg = calloc(1, sizeof(sync_msg_t));
        synchronized = 0;

        refresh_fd_set(&readfds); /*Copy the entire monitored FDs to readfds*/

        printf("Please select from the following options:\n");
        printf("1)CREATE <Data>\n");
        printf("2)DELETE <Data>\n");
        printf("3)SHOW TABLE\n");
        printf("4)FLUSH TABLE\n");

        select(get_max_fd() + 1, &readfds, NULL, NULL, NULL);  /* Wait for incoming connections. */

        /* New connection: send entire hash table and data list states to newly connected client. */
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

            add_to_monitored_fd_set(data_socket);
            add_to_client_pid_set(pid);


            update_new_client(data_socket, HASH_L, op, sync_msg);
            update_new_client(data_socket, DATA_L, op, sync_msg);
        }
        else if(FD_ISSET(0, &readfds)){ // server stdin
            ret = read(0, op, OP_LEN - 1);

            op[strcspn(op, "\r\n")] = 0; // flush new line
            if (ret == -1) {
                perror("read");
                return 1;
            }
            op[ret] = 0;

            if (!create_sync_message(op, sync_msg, 0)) {
                // update server's tables
                if (sync_msg->l_code == HASH_L) {
                    process_sync_mesg(hash_table, sync_msg);
                    synchronized = HASH_LIST;
                }
                else {
                    process_sync_mesg(data_list, sync_msg);
                    synchronized = DATA_LIST;
                }

                /* Notify existing clients of changes */
                int i, comm_socket_fd;
                for (i = 2; i < MAX_CLIENTS; i++) { // start at 2 since 0 and 1 are for server's stdin and stdout
                    comm_socket_fd = monitored_fd_set[i];
                    if (comm_socket_fd != -1) {
                        write(comm_socket_fd, sync_msg, sizeof(sync_msg_t));
                        write(comm_socket_fd, &synchronized, sizeof(int));
                        write(comm_socket_fd, &loop, sizeof(int));
                    }
                }
            }
        }
        else { /* Check active status of clients */
            int i;
            for(i = 2; i < MAX_CLIENTS; i++){
                if(FD_ISSET(monitored_fd_set[i], &readfds)){
                    int done;
                    int comm_socket_fd = monitored_fd_set[i];

                    ret = read(comm_socket_fd, &done, sizeof(int));
                    if (done == 1) { // this client is disconnecting
                        close(comm_socket_fd);
                        remove_from_monitored_fd_set(comm_socket_fd);
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
    exit(0);
}
