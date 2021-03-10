/* send_file.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "send_file.h"

/* Internal struct to maintain the status of a file transfer */
typedef struct {
    file_t *file_info;
    int my_socket;
    int already_sent;
    char *send_buffer;
} file_status_t;

void start_file_transfer(void *file_info_in, unsigned int task_id){
    file_t *file_info = (file_t *) file_info_in;
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
    //sin.sin_addr.s_addr = file_info->remote_addr;
    sin.sin_addr.s_addr = 16777343; // XXX Fix-me: parsing the remote_addr has a bug
    sin.sin_port = htons(file_info->remote_port);

    /* connect to the server */
    if (connect(my_socket, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
        perror("error connecting to the server");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    //Testing up to this point: communicate with the example server
    /* everything looks good, since we are expecting a
     message from the server in this example, let's try receiving a
     message from the socket. this call will block until some data
     has been received */
    printf("This socket has id: %d\n", my_socket);
    char *buffer;
    int size = 500;
    buffer = (char *) malloc(size);
    int count = recv(my_socket, buffer, size, 0);
    if (count < 0)
        {
        perror("receive failure");
        abort();
        }

    /* in this simple example, the message is a string, 
        we expect the last byte of the string to be 0, i.e. end of string */
    if (buffer[count-1] != 0)
        {
        /* In general, TCP recv can return any number of bytes, not
        necessarily forming a complete message, so you need to
        parse the input to see if a complete message has been received.
            if not, more calls to recv is needed to get a complete message.
        */
        printf("Message incomplete, something is still being transmitted\n");
        } 
    else
        {
        printf("Here is what we got: %s", buffer);
        }
    ///////////////////////////////////////////////////////////////////////

    /* Send the first chunck with size information */
    // XXX write the logic to send first chunk with small header
    // file_status->already_sent += sent_bytes;

    //Schedule the send another chunk task for this file here??? Yes!

    //Kill this task
    kill_task(task_id);

    return;
}

void send_file_chunk(void *file_status, unsigned int task_id) {
    return;
}

void clean_up_file_transfer(void *file_status, unsigned int task_id) {
    return;
}