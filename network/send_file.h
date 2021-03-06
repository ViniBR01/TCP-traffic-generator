/* send_file.h - TCP traffic generator */

/* Public struct with the minimum information to call this component */
typedef struct {
    uint64_t        file_size;
    int             max_chunk_size;
    char           *remote_addr;
    unsigned short  remote_port;
    uint32_t        mode;
    uint32_t        period_ms;
    uint32_t        unique_id;
} file_t;

void start_file_transfer(void *file_info_in, unsigned int task_id);
void send_file_chunk(void *file_status, unsigned int task_id);
void clean_up_file_transfer(void *file_status, unsigned int task_id);