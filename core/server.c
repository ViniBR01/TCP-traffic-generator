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

#include "server.h"
#include "traffic_factory.h"
#include "scheduler.h"

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
  int period = (int) options->period;
  int fsize = options->file_size;
  int portn = options->port;

  //start all traffic model instances
  // A traffic generator is resposible to create files for transmissions
  //They provede interface to create a traffic generation task. This task
  //creates files with a specified size in kB and at specified time.
  //1 - fixed periodic:
  // traffic_factory_ops traffic_factory = get_traffic_factory(FIXED_PERIODIC);
  // ctx_t *traffic_generator = traffic_factory->create(period, fsize, portn);
  //eventually, do: traffic_factory->destroy(&traffic_generator);

  //Proof of concept
  unsigned int *task_parameter1 = (unsigned int *) malloc(sizeof(unsigned int));
  *task_parameter1 = (unsigned int) (1000*period);
  create_task(periodic_task, (void *)task_parameter1, STATE_WAITING, 1000000);

  //Now start scheduler
  while(1) {
    scheduler();
  }

  return EXIT_SUCCESS;
}

void periodic_task(void *p, unsigned int task_id) {
  int time = get_scheduler_time_msec();
  printf("t=%4.d ms | Executing the periodic task with id: %d.\n", time, task_id);

  create_task(send_file_task, NULL, STATE_WAITING, 50000);

  delay(*(unsigned int *)p, task_id);
  return;
}

void send_file_task(void *p, unsigned int task_id) {
  int time = get_scheduler_time_msec();
  printf("t=%4.d ms | Send a file starting now. This task id is: %d\n", time, task_id);
  kill_task(task_id);
  return;
}