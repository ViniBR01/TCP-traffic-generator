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
#include "traffic_factory.h"
#include "config_messages.h"

int server(options_t *options) {
  printf("Inside of the server function.\n");

  if (!options) {
    errno = EINVAL;
    return EXIT_FAILURE;
  }

  /* Select the operation mode and do server stuff */
  switch (options->mode) {
  
    case MODE_SINGLE_UL: ;
      /* Prepare the setup configuration with the structs from config_messages.h */
      single_model_t model1;
      model1.model_type = FIXED_PERIODIC;
      model1.execution_time = options->length_sec;
      model1.period_ms = options->period_ms;
      model1.file_size_kb = options->file_size_kb;
      model1.port = options->port;

      setup_t configs;
      configs.number_of_models = 1;
      configs.model_array = &model1;

      /* Send a silly message to the client */
      message_t *message_arg = cook_setup_message(MSGTYPE_SETUP, configs, 
                                            options->remote_ip, options->port);
      create_task(send_message, message_arg, STATE_READY, -1);
      break;
    
    case MODE_SINGLE_DL: ;
      /* start all traffic model instances */
      // A traffic generator is resposible for creating files for transmissions
      //They provide interface to create a traffic generation task. This task
      //creates files with a specified size in kB and at specified time.
      //1 - fixed periodic:
      // traffic_factory_ops traffic_factory = get_traffic_factory(FIXED_PERIODIC);
      // ctx_t *traffic_generator = traffic_factory->create(period, fsize, portn);
      //eventually, do: traffic_factory->destroy(&traffic_generator);

      //Currently, the implementation does not use the factory method: XXX
      traffic_t *traffic_arg = (traffic_t *)malloc(sizeof(traffic_t));
      traffic_arg->model_type = FIXED_PERIODIC;
      traffic_arg->execution_time_sec = options->length_sec;
      traffic_arg->period_ms = options->period_ms;
      traffic_arg->execution_count = 1000*traffic_arg->execution_time_sec / options->period_ms;
      traffic_arg->file_size_kb = options->file_size_kb;
      traffic_arg->remote_ip = options->remote_ip;
      traffic_arg->port = options->port;
      traffic_arg->mode = options->mode;
      struct timeval id_time;
      gettimeofday(&id_time, NULL);
      traffic_arg->unique_id = (uint32_t )id_time.tv_sec;

      int retval = create_traffic(traffic_arg);
      break;

    default:
      break;
  }

  /* Now, start scheduler */
  while(1) {
    scheduler();
  }

  return EXIT_SUCCESS;
}