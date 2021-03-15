/* config_messages.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "config_messages.h"

message_t * cook_message(u_int8_t type, u_int32_t length, char *string_message) {
    message_t *message_arg = (message_t *) malloc(sizeof(message_t));
    message_arg->message_type = type;
    message_arg->message_length = length;
    message_arg->buffer = (char *)malloc(length * sizeof(char));

    //XX Here, fill buffer with silly message and header

    return message_arg;
}
void send_message(void *cooked_message, unsigned int task_id) {
    
    return;
}