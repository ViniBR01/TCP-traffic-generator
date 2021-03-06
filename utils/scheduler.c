/* scheduler.c - TCP traffic generator */

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

#include "scheduler.h"

int task_index = 0;
tcb TaskList[N_MAX_TASKS];
// void (*readytasks[N_MAX_TASKS]) ();
// void (*haltedtasks[N_MAX_TASKS]) ();
// void (*sleepingtasks[N_MAX_TASKS]) ();

void scheduler(){
  if(TaskList[task_index].ftpr == NULL && task_index != 0) {
    task_index = 0;
  }
  //if(TaskList[task_index].ftpr == NULL && task_index == 0) {
  //	printf("No tasks exist.\n");/*No tasks!*/
  if(TaskList[task_index].state == STATE_READY) {
    printf("Function scheduler(); task_index = %d; State_ready\n", task_index);
    start_task(TaskList[task_index].ftpr, TaskList[task_index].arg_ptr);
  }else if(TaskList[task_index].state == STATE_WAITING) {
    //printf("Function scheduler(); task_index = %d; State_waiting\n", task_index);
    struct timeval currentTime;
    struct timeval elapsedTime;
    gettimeofday(&currentTime, NULL);
    timersub(&currentTime, &TaskList[task_index].delay_ref, &elapsedTime);
    unsigned long time_in_micros = 1000000*elapsedTime.tv_sec + elapsedTime.tv_usec;
    if ((unsigned long)TaskList[task_index].delay <= time_in_micros) {
      TaskList[task_index].state = STATE_READY;
      TaskList[task_index].delay = -1;
    }
  }
  task_index++;
  return;
}

void start_task(void (*functionPTR)(void *) , void* param_ptr){
  functionPTR(param_ptr);
}

void halt_me(){
  //Deactivates one task - the current one
  TaskList[task_index].state = STATE_INACTIVE;
}

void delay(unsigned int usec){
  //Sets a delay value for tasks in microsecs
  TaskList[task_index].state = STATE_WAITING;
  TaskList[task_index].delay = usec;
  gettimeofday(&TaskList[task_index].delay_ref, NULL);
}