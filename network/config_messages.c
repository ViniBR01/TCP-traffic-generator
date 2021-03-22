/* config_messages.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>

#include "config_messages.h"
#include "scheduler.h"
#include "verbosity.h"

int serialize_setup_message(setup_t *setup_message, char *buffer);
int reconstruct_setup_message(char* buffer, setup_t *setup_message);

message_t * cook_setup_message(u_int8_t type, setup_t setup_configs, 
                            char *remote_ip, uint32_t port) {
    message_t *message_arg = (message_t *) malloc(sizeof(message_t));
    message_arg->message_type = type;
    int message_size = 1 + 4 + sizeof(setup_t) + 
                        setup_configs.number_of_models * sizeof(single_model_t);
    message_arg->message_length = message_size; //XXX fix-me: later
    message_arg->buffer = (char *)malloc(message_size * sizeof(char));
    if(!message_arg->buffer) {
        perror("error allocating buffer for config message");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }
    message_arg->remote_ip = (char *) malloc(16);
    strcpy(message_arg->remote_ip, remote_ip);
    message_arg->port = port;

    //XX Here, fill buffer with setup message
    message_arg->buffer[0] = type;
    *(uint32_t *) (message_arg->buffer + 1) = (uint32_t) htonl(message_size);
    //XXX must use a function to serialize the control structs to be sent
    serialize_setup_message(&setup_configs, message_arg->buffer+5);

    return message_arg;
}

void send_message(void *cooked_message, unsigned int task_id) {

    message_t *message_arg = (message_t *) cooked_message;
    //XXX Write code to open a socket, connect to remote host, and send the contents of the buffer
    // printf("A silly message testing.\n");
    // printf("type = %d\n", *(uint8_t *) ((message_t *) cooked_message)->buffer);
    // printf("length = %d\n", ntohl(*(uint32_t *) (((message_t *) cooked_message)->buffer+1) ) );
    // printf("%c\n", message_arg->buffer[5]);

    int my_socket;
    if ((my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("error opening TCP socket.");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    /* disable nagle's algorithm to guarantee max speed */
    int optval = 1;
    if(setsockopt(my_socket,
                    IPPROTO_TCP,
                    TCP_NODELAY,
                    (char *) &optval,
                    sizeof(int)) < 0) {
        perror("error setting nagle's option");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    /* fill in the server's address */
    struct sockaddr_in sin;
    memset (&sin, 0, sizeof (sin));
    sin.sin_family = AF_INET;
    inet_aton(message_arg->remote_ip, &sin.sin_addr);
    sin.sin_port = htons(message_arg->port);

    /* connect to the server */
    if (connect(my_socket, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
        perror("error connecting to the server");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    int send_size = message_arg->message_length;
    int count = send(my_socket, message_arg->buffer, send_size, MSG_DONTWAIT);
    if (count < 0) {
        if (errno == EAGAIN) {
        /* we are trying to dump too much data down the socket,
            it cannot take more for the time being 
            will have to go back to select and wait til select
            tells us the socket is ready for writing
        */
            perror("error sending config message to server.");
            exit(EXIT_FAILURE);
            /* NOTREACHED */
        } else {
            /* something else is wrong */
            perror("error sending message to server.");
            exit(EXIT_FAILURE);
            /* NOTREACHED */
        }
    }

    /* Cleanup connection */
    close(my_socket);
    free(message_arg->buffer);
    free(message_arg);

    kill_task(task_id);
    return;
}

//XXX Fix me: the following functions only work for a single model!!!
int serialize_setup_message(setup_t *setup_message, char *buffer) {
    //must take all the content of setup message and add it to buffer
    *(uint32_t *) (buffer+0) = (uint32_t) htonl(setup_message->number_of_models);
    for (int i=0; i<setup_message->number_of_models; i++) {
        *(int *) (buffer+4+i*24) = (int) htonl(setup_message->model_array->model_type);
        *(int *) (buffer+8+i*24) = (int) htonl(setup_message->model_array->execution_time);
        *(uint32_t *) (buffer+12+i*24) = (uint32_t) htonl(setup_message->model_array->period_ms);
        *(uint32_t *) (buffer+16+i*24) = (uint32_t) htonl(setup_message->model_array->file_size_kb);
        *(uint32_t *) (buffer+20+i*24) = (uint32_t) htonl(setup_message->model_array->port);
    }
    return EXIT_SUCCESS;
}
int reconstruct_setup_message(char* buffer, setup_t *setup_message) {
    uint32_t number_of_models = (uint32_t) ntohl(*(uint32_t *)(buffer+0));
    setup_message->number_of_models = number_of_models;
    for (int i=0; i<number_of_models; i++) {
        setup_message->model_array->model_type = (int) ntohl(*(int *)(buffer+4+i*24));
        setup_message->model_array->execution_time = (int) ntohl(*(int *)(buffer+8+i*24));
        setup_message->model_array->period_ms = (uint32_t) ntohl(*(uint32_t *)(buffer+12+i*24));
        setup_message->model_array->file_size_kb = (uint32_t) ntohl(*(uint32_t *)(buffer+16+i*24));
        setup_message->model_array->port = (uint32_t) ntohl(*(uint32_t *)(buffer+20+i*24));
    }
    return EXIT_SUCCESS;
}