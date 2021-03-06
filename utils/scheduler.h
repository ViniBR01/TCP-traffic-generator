/* scheduler.h - TCP traffic generator */

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <time.h>
#include <sys/time.h>

#define STATE_READY 1
#define STATE_WAITING 2
#define STATE_INACTIVE 3
#define N_MAX_TASKS 10

typedef struct TCBstruct {
  void (*ftpr)(void *p);    //the function pointer
  void *arg_ptr;			//the argument pointer
  unsigned short int state;	//the task state
  unsigned int delay;		//sleep delay in microseconds
  struct timeval delay_ref;	//reference time when delay started
} tcb;

extern tcb TaskList[N_MAX_TASKS];
extern int task_index;

int create_task(void (*function_ptr)(void *p), void *argument_ptr, unsigned short int state, unsigned int delay);
void scheduler();
void halt_me();
void delay(unsigned int usec);

#endif  /* _SERVER_H */