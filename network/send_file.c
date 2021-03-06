/* send_file.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>

#include "send_file.h"
#include "scheduler.h"
#include "log_file.h"
#include "verbosity.h"

/* Internal struct to maintain the status of a file transfer */
typedef struct {
    file_t *file_info;
    int     my_socket;
    int     already_sent;
    char   *send_buffer;
    int     start_time_usec;
} file_status_t;

void fill_buffer(char *buffer, int length);
int mymin(uint64_t num1, int num2);

void start_file_transfer(void *file_info_in, unsigned int task_id){
    verbosity("Inside of the start_file_transfer function.\n", 3);

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

    /* disable nagle's algorithm to guarantee max speed */
    int optval =1;
    if(setsockopt(my_socket,
                    IPPROTO_TCP,
                    TCP_NODELAY,
                    (char *) &optval,
                    sizeof(int)) < 0) {
        perror("error setting nagle's option");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    /* Try to set intial window size to ~2kB */
    // optval = 4096;
    // if(setsockopt(my_socket,
    //                 SOL_SOCKET,
    //                 SO_RCVBUF,
    //                 &optval,
    //                 sizeof(int)) < 0) {
    //     perror("error setting recv buf size");
    //     exit(EXIT_FAILURE);
    //     /* NOTREACHED */
    // }
    // if(setsockopt(my_socket,
    //                 SOL_SOCKET,
    //                 SO_SNDBUF,
    //                 &optval,
    //                 sizeof(int)) < 0) {
    //     perror("error setting send buf size");
    //     exit(EXIT_FAILURE);
    //     /* NOTREACHED */
    // }

    file_status->my_socket = my_socket;
    file_status->start_time_usec = get_scheduler_time_usec();

    /* fill in the server's address */
    struct sockaddr_in sin;
    memset (&sin, 0, sizeof (sin));
    sin.sin_family = AF_INET;
    inet_aton(file_info->remote_addr, &sin.sin_addr);
    sin.sin_port = htons(file_info->remote_port);

    /* connect to the server */
    if (connect(my_socket, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
        perror("error connecting to the server");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

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

void send_file_chunk(void *file_status_in, unsigned int task_id) {
    verbosity("Inside of the send_file_chunk function.\n", 3);

    /* Here we have access to a socket, a sendbuffer, a file size and how much was already sent */
    /* Check if file has already finished transmitting */
    file_status_t *file_status = (file_status_t *) file_status_in;
    uint64_t remaining_size = (uint64_t) file_status->file_info->file_size - (uint64_t) file_status->already_sent;

    if (remaining_size == 0) {
        /* Check if socket buffer is empty or still has data to be consumed */
        int pending = 0;
        int err = ioctl(file_status->my_socket, SIOCOUTQ, &pending);
        if (err < 0) {
            perror("error checking socket send buffer.");
            exit(EXIT_FAILURE);
            /* NOTREACHED */
        }

        verbosity_int("\tPending data = %d\n", pending, 5);

        if (pending == 0) {
            //end of transmission XXX

            //calculate time to send file:
            int total_time_to_send = get_scheduler_time_usec() - file_status->start_time_usec;
            verbosity_int("Download time = %d usec\n", total_time_to_send, 2);
            int retval = log_int(total_time_to_send, 
                                    (uint8_t )file_status->file_info->mode, 
                                    file_status->file_info->period_ms, 
                                    file_status->file_info->file_size, 
                                    file_status->file_info->unique_id);
            create_task(clean_up_file_transfer, file_status_in, STATE_READY, -1);
            kill_task(task_id);
        }

    } else {
        //send another chunk
        int send_size = mymin(remaining_size, file_status->file_info->max_chunk_size);
        int count = send(file_status->my_socket, file_status->send_buffer,
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
            file_status->already_sent += (uint64_t)count;
        }
    }
    return;
}

void clean_up_file_transfer(void *file_status, unsigned int task_id) {
    verbosity("Inside of the clean_up_file_transfer function.\n", 3);

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

int mymin(uint64_t num1, int num2) {
    if (num1 >= (uint64_t)num2) {
        return num1;
    } else {
        return num2;
    }
}