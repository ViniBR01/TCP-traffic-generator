/* server.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netdb.h>

#include "server.h"
#include "scheduler.h"
//#include "traffic_factory.h"
#include "send_file.h"

typedef struct {
  uint32_t period;
  uint32_t fsize;
} periodic_arg_t;

void periodic_task(void *p, unsigned int task_id);
void send_file_task(void *p, unsigned int task_id);

int server(options_t *options) {
  printf("Inside of the server function.\n");

  if (!options) {
    errno = EINVAL;
    return EXIT_FAILURE;
  }

  /* XXX do server stuff */
  //Read arguments from options
  uint32_t period = options->period;
  uint32_t fsize = options->file_size;
  uint32_t portn = options->port;

  //start all traffic model instances
  // A traffic generator is resposible to create files for transmissions
  //They provede interface to create a traffic generation task. This task
  //creates files with a specified size in kB and at specified time.
  //1 - fixed periodic:
  // traffic_factory_ops traffic_factory = get_traffic_factory(FIXED_PERIODIC);
  // ctx_t *traffic_generator = traffic_factory->create(period, fsize, portn);
  //eventually, do: traffic_factory->destroy(&traffic_generator);

  //Proof of concept
  periodic_arg_t *task1_arg = (periodic_arg_t *) malloc(sizeof(periodic_arg_t));
  task1_arg->period = (uint32_t) (1000*period);
  task1_arg->fsize = (uint32_t) (1024*fsize);
  create_task(periodic_task, (void *)task1_arg, STATE_WAITING, 1000000);

  //Now start scheduler
  while(1) {
    scheduler();
  }

  return EXIT_SUCCESS;
}

void periodic_task(void *p, unsigned int task_id) {
  int time = get_scheduler_time_msec();
  printf("t=%4.d ms | Executing the periodic task with id: %d.\n", time, task_id);

  create_task(send_file_task, p, STATE_WAITING, 50000);

  periodic_arg_t *arg = (periodic_arg_t *) p;
  delay(arg->period, task_id);
  return;
}

void send_file_task(void *p, unsigned int task_id) {
  int time = get_scheduler_time_msec();
  periodic_arg_t *arg = (periodic_arg_t *) p;
  printf("t=%4.d ms | Send a file starting now. File_size=%u | This task id is: %d\n", time, arg->fsize, task_id);

  //Here should call a function that starts a non-blocking file transmission
  file_t *file_info = (file_t *) malloc(sizeof(file_t));
  file_info->file_size = arg->fsize;
  file_info->max_chunk_size = 1500;

  unsigned int server_addr;
  struct addrinfo *getaddrinfo_result, hints;
  if (getaddrinfo("127.0.0.1", NULL, &hints, &getaddrinfo_result) == 0) {
    server_addr = (unsigned int) ((struct sockaddr_in *) 
                  (getaddrinfo_result->ai_addr))->sin_addr.s_addr;
    freeaddrinfo(getaddrinfo_result);
  }

  file_info->remote_addr = server_addr; //hard-coded to be local server
  file_info->remote_port = 8080;
  create_task(start_file_transfer, (void *) file_info, STATE_READY, -1);
  //////////////////////////////////////////////////////////////////////////

  kill_task(task_id);
  return;
}