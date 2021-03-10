/* send_file.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "send_file.h"

typedef struct {
    file_t *file_info;
    int my_socket;
    int already_sent;
    char *send_buffer;
} file_status_t;

void * start_file_transfer(file_t *file_info){
    file_status_t *file_status = 
        (file_status_t *) malloc(sizeof(file_status_t));
    if (!file_status) {
        perror("failed to allocate file_status struct.");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    file_status->file_info = file_info;
    file_status->already_sent = 0;
    file_status->send_buffer = 
        (char *) malloc(file_status->file_info->max_chunk_size * sizeof(char));
    if (!file_status->send_buffer) {
        perror("failed to allocate sendbuffer.");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }
    
    int my_socket;
    if ((my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("error opening TCP socket.");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }
    file_status->my_socket = my_socket;

    /* fill in the server's address */
    struct sockaddr_in sin;
    memset (&sin, 0, sizeof (sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = file_info->remote_addr;
    sin.sin_port = htons(file_info->remote_port);

    /* connect to the server */
    if (connect(my_socket, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
        perror("error connecting to the server");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    /* Send the first chunck with size information */
    // XXX write the logic to send first chunk with small header
    // file_status->already_sent += sent_bytes;

    return (void *) file_status;
}

int send_file_chunk(void *file_status) {
    return EXIT_SUCCESS;
}

int clean_up_file_transfer(void *file_status) {
    return EXIT_SUCCESS;
}