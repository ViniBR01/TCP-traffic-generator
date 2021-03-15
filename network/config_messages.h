/* config_messages.h - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define MSGTYPE_SETUP 1
#define MSGTYPE_DIGEST 2

typedef struct {
    uint8_t message_type;
    uint32_t message_length;
    char *buffer;
} message_t;

message_t * cook_message(u_int8_t type, u_int32_t length, char *string_message);
void send_message(void *cooked_message, unsigned int task_id);