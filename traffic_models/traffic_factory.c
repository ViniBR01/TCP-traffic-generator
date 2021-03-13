/* traffic_factory.c - TCP traffic generator */

#include <stdio.h>  //FILE, stdin, stdout, stderr, and the fprint() family
#include <stdlib.h> //malloc(), calloc(), and realloc()
#include <stdint.h> //special types such as uint32_t
#include <unistd.h> //EXIT_FAILURE, EXIT_SUCCESS
#include <libgen.h> //basename() function
#include <errno.h>  //defines external errno var and the values it can take on
#include <string.h> //memcpy(), memset(), and the strlen() family
#include <getopt.h> //external optarg, opterr, optind, and getopt() function

#include "traffic_factory.h"
//#include "m_fixed_periodic.h"
#include "scheduler.h"
#include "send_file.h"

void periodic_task(void *p, unsigned int task_id);
void send_file_task(void *p, unsigned int task_id);

int create_traffic(traffic_t *traffic_parameters) {
    //for the fixed periodic, must create a periodic task
    create_task(periodic_task, (void *)traffic_parameters, STATE_WAITING, 1000000);
    return EXIT_SUCCESS;
}

void periodic_task(void *p, unsigned int task_id) {
    //int time = get_scheduler_time_usec();
    //printf("t=%4.d ms | Executing the periodic task with id: %d.\n", time, task_id);

    traffic_t *arg = (traffic_t *) p;
    /* Get a random phase between 0 and T */
    unsigned int phase_delay = rand() % 1000*arg->period_ms;

    create_task(send_file_task, p, STATE_WAITING, phase_delay);

    delay(1000*arg->period_ms, task_id);
    return;
}

void send_file_task(void *p, unsigned int task_id) {
    //int time = get_scheduler_time_usec();
    //printf("t=%4.d ms | Send a file starting now. File_size=%u | This task id is: %d\n", time, arg->fsize, task_id);
    traffic_t *arg = (traffic_t *) p;

    file_t *file_info = (file_t *) malloc(sizeof(file_t));
    file_info->file_size = 1024*arg->file_size_kb;
    file_info->max_chunk_size = 1500;
    file_info->remote_addr = arg->remote_ip;
    file_info->remote_port = arg->port;
    create_task(start_file_transfer, (void *) file_info, STATE_READY, -1);
    
    kill_task(task_id);
    return;
}