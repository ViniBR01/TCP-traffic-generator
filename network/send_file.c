/* send_file.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>

#include "send_file.h"

typedef struct {
    file_t file_info;
    int my_socket;
    char *send_buffer;
} file_status_t;

void * start_file_transfer(file_t file_info){
    file_status_t *file_status;
    return file_status;
}

int send_file_chunk(void * file_status) {
    return EXIT_SUCCESS;
}

int clean_up_file_transfer(void * file_status) {
    return EXIT_SUCCESS;
}