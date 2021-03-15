/* config_messages.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <arpa/inet.h>

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

    //XXX Write code to open a socket, connect to remote host, and send the contents of the buffer
    printf("A silly message testing.\n");
    printf("type = %d\n", *(uint8_t *) ((message_t *) cooked_message)->buffer);
    printf("length = %d\n", ntohl(*(uint32_t *) (((message_t *) cooked_message)->buffer+1) ) );
    
    kill_task(task_id);
    return;
}