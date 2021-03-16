/* config_messages.h - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define MSGTYPE_SETUP 1
#define MSGTYPE_DIGEST 2

/* Struct to control the message transmission */
typedef struct {
    uint8_t message_type;
    uint32_t message_length;
    char *buffer;
    char *remote_ip;
    uint32_t port;
} message_t;

/* Structs to exchange the configuration setup parameters */
typedef struct {
    uint8_t model_type;
    int execution_time;
    int period;
    int file_size;
    int port;
} single_model_t;

typedef struct {
    int number_of_models;
    single_model_t *model_array;
} setup_t;

message_t * cook_message(u_int8_t type, u_int32_t length, char *string_message, char *remote_ip, uint32_t port);
void send_message(void *cooked_message, unsigned int task_id);