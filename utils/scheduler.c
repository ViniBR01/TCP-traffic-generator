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

#define N_MAX_TASKS 10

typedef struct TCBstruct {
  void (*ftpr)(void *p, unsigned int task_id);    //the function pointer
  void *arg_ptr;			//the argument pointer
  unsigned short int state;	//the task state
  unsigned int delay;		//sleep delay in microseconds
  struct timeval delay_ref;	//reference time when delay started
} tcb;

tcb TaskList[N_MAX_TASKS];
unsigned int task_index = 0;
unsigned int creating_id = 0;
// void (*readytasks[N_MAX_TASKS]) ();
// void (*haltedtasks[N_MAX_TASKS]) ();
// void (*sleepingtasks[N_MAX_TASKS]) ();

unsigned int create_task( \
    void (*function_ptr)(void *p, unsigned int task_id), \
    void *argument_ptr, unsigned short int state, unsigned int delay) {
  unsigned int j = creating_id;
  TaskList[j].ftpr = function_ptr;
  TaskList[j].arg_ptr = (void *) argument_ptr;
  TaskList[j].state = state;
  TaskList[j].delay = delay;

  creating_id++;
  TaskList[creating_id].ftpr = NULL;
}

static void start_task(void (*functionPTR)(void *, unsigned int) , void* param_ptr, unsigned int task_id){
  functionPTR(param_ptr, task_id);
}

void scheduler(){
  if(TaskList[task_index].ftpr == NULL && task_index != 0) {
    task_index = 0;
  }
  //if(TaskList[task_index].ftpr == NULL && task_index == 0) {
  //	printf("No tasks exist.\n");/*No tasks!*/
  if(TaskList[task_index].state == STATE_READY) {
  //  printf("Function scheduler(); task_index = %d; State_ready\n", task_index);
    start_task(TaskList[task_index].ftpr, TaskList[task_index].arg_ptr, task_index);
  }else if(TaskList[task_index].state == STATE_WAITING) {
  //  printf("Function scheduler(); task_index = %d; State_waiting\n", task_index);
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

void halt_me(unsigned int task_id){
  //Deactivates one task - the current one
  TaskList[task_id].state = STATE_INACTIVE;
}

void delay(unsigned int usec, unsigned int task_id){
  //Sets a delay value for tasks in microsecs
  TaskList[task_id].state = STATE_WAITING;
  TaskList[task_id].delay = usec;
  gettimeofday(&TaskList[task_id].delay_ref, NULL);
}