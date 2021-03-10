/* send_file.h - TCP traffic generator */

typedef struct {
    int file_size;
    unsigned int remote_addr;
    unsigned short remote_port;
} file_t;

void * start_file_transfer(file_t file_info);
int send_file_portion(void * file_status);
int clean_up_file_transfer(void * file_status);