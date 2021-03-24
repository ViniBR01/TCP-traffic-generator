/* open_server.h - TCP traffic generator */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

/* A linked list node data structure to maintain application
   information related to a connected socket */
struct node {
  int socket;
  struct sockaddr_in client_addr;
  int pending_data;
  int is_file_transfer;
  
  struct node *next;
};

/* struct type for the open_server argument */
typedef struct {
    int socket;
    char *recv_buffer;
    struct node *open_sockets;
} server_t;

server_t * setup_server(unsigned int port);
void check_connections(void *server_options, unsigned int task_id);