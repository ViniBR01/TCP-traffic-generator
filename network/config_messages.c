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

message_t * cook_message(u_int8_t type, u_int32_t length, char *string_message, char *remote_ip, uint32_t port) {
    message_t *message_arg = (message_t *) malloc(sizeof(message_t));
    message_arg->message_type = type;
    message_arg->message_length = length;
    message_arg->buffer = (char *)malloc(length * sizeof(char));
    if(!message_arg->buffer) {
        perror("error allocating buffer for config message");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }
    message_arg->remote_ip = remote_ip;
    message_arg->port = port;

    //XX Here, fill buffer with silly message and header
    message_arg->buffer[0] = type;
    *(uint32_t *) (message_arg->buffer + 1) = (uint32_t) htonl(length);
    memcpy(message_arg->buffer+5, string_message, length-5);

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