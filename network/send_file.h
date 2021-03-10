/* send_file.h - TCP traffic generator */

/* Public struct with the minimum information to call this component */
typedef struct {
    int file_size;
    int max_chunk_size;
    unsigned int remote_addr;
    unsigned short remote_port;
} file_t;

int start_file_transfer(void *file_info_in, unsigned int task_id);
int send_file_chunk(void *file_status, unsigned int task_id);
int clean_up_file_transfer(void *file_status, unsigned int task_id);