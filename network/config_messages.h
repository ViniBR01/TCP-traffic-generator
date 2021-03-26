/* config_messages.h - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define MSGTYPE_SETUP 93
#define MSGTYPE_DIGEST 94
#define MSGTYPE_FILE_TRANSFER 95

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
    int model_type;
    int execution_time;
    uint32_t period_ms;
    uint32_t file_size_kb;
    uint32_t port;
} single_model_t;

typedef struct {
    uint32_t number_of_models;
    single_model_t *model_array;
} setup_t;

message_t * cook_setup_message(u_int8_t type, setup_t setup_configs, 
                            char *remote_ip, uint32_t port);
void send_message(void *cooked_message, unsigned int task_id);
int reconstruct_setup_message(char* buffer, setup_t *setup_message);