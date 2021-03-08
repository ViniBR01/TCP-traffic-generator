/* scheduler.h - TCP traffic generator */

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <time.h>
#include <sys/time.h>

#define STATE_READY 1
#define STATE_WAITING 2
#define STATE_INACTIVE 3

typedef struct TASKstruct {
  unsigned int task_id;
  void (*function_ptr)(void *p, unsigned int task_id);
  void *arg_ptr;
  unsigned short int state;
  int delay;
  struct timeval delay_ref;	  //reference time when delay started
} task_t;

typedef struct TCBstruct {
  task_t *task_list;
  int capacity;
  int count;
  int iterator;
} tcb_t;

extern tcb_t ready_tasks;
extern tcb_t halted_tasks;
extern tcb_t sleeping_tasks;

unsigned int create_task(void (*function_ptr)(void *p, unsigned int task_id), 
                          void *argument_ptr, unsigned short int state, unsigned int delay);
int kill_task(unsigned int task_id);
void scheduler();
void halt_me(unsigned int task_id);
void delay(int new_delay, unsigned int task_id); //delay in usec
int get_scheduler_time();

#endif  /* _SERVER_H */