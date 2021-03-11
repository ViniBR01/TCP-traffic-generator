/* send_file.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "send_file.h"
#include "scheduler.h"

/* Internal struct to maintain the status of a file transfer */
typedef struct {
    file_t *file_info;
    int my_socket;
    int already_sent;
    char *send_buffer;
    int start_time_usec;
} file_status_t;

void fill_buffer(char *buffer, int length);
int mymin(int num1, int num2);

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

    file_status->start_time_usec = get_scheduler_time_usec();

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
    if (count < 0) {
        perror("receive failure.");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    /* in this simple example, the message is a string, 
        we expect the last byte of the string to be 0, i.e. end of string */
    if (buffer[count-1] != 0) {
        /* In general, TCP recv can return any number of bytes, not
        necessarily forming a complete message, so you need to
        parse the input to see if a complete message has been received.
            if not, more calls to recv is needed to get a complete message.
        */
        printf("Message incomplete, something is still being transmitted\n");
    } else {
        printf("Here is what we got: %s", buffer);
    }
    ///////////////////////////////////////////////////////////////////////

    /* Send the first chunck with size information */
    // XXX write the logic to send first chunk with small header
    fill_buffer(file_status->send_buffer, file_status->file_info->max_chunk_size); //XXX write this function
    // file_status->already_sent += sent_bytes;

    //Schedule the send another chunk task for this file here??? Yes! XXX
    create_task(send_file_chunk, (void *) file_status, STATE_READY, -1);

    //Kill this task
    kill_task(task_id);

    return;
}

void send_file_chunk(void *file_status, unsigned int task_id) {
    /* Here we have access to a socket, a sendbuffer, a file size and how much was already sent */
    //First, check if file has already finished transmitting
    file_status_t *file_status_in = file_status;
    int remaining_size = (int) file_status_in->file_info->file_size - (int) file_status_in->already_sent;
    if (remaining_size <= 0) {
        //end of transmission XXX

        //calculate time to send file:
        int total_time_to_send = get_scheduler_time_usec() - file_status_in->start_time_usec;
        printf("\tTime to transmit file: %d ms\n", total_time_to_send);
        
        create_task(clean_up_file_transfer, (void *) file_status, STATE_WAITING, 1000000);
        kill_task(task_id);
        //
    } else {
        //send another chunk
        int send_size = mymin(remaining_size, file_status_in->file_info->max_chunk_size);
        int count = send(file_status_in->my_socket, file_status_in->send_buffer,
                            send_size, MSG_DONTWAIT);
        if (count < 0) {
            if (errno == EAGAIN) {
            /* we are trying to dump too much data down the socket,
                it cannot take more for the time being 
                will have to go back to select and wait til select
                tells us the socket is ready for writing
            */
            } else {
                /* something else is wrong */
                perror("error sending message to server.");
                exit(EXIT_FAILURE);
                /* NOTREACHED */
            }
        } else {
            //account for amount of data sent
            file_status_in->already_sent += count;
        }
    }
    return;
}

void clean_up_file_transfer(void *file_status, unsigned int task_id) {

    file_status_t *file_status_in = file_status;
    
    //close socket
    close(file_status_in->my_socket);
    //free sendbuffer
    free(file_status_in->send_buffer);
    //free file_info and file_status
    file_t *file_info_in = file_status_in->file_info;
    free(file_info_in);
    free(file_status_in);

    //kill this task
    kill_task(task_id);

    return;
}

void fill_buffer(char *buffer, int length) {
    for(int i=0; i<length; i++) {
        buffer[i] = 'v';
    }
    return;
}

int mymin(int num1, int num2) {
    if (num1 >= num2) {
        return num1;
    } else {
        return num2;
    }
}