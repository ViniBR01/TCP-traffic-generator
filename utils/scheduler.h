/* scheduler.h - TCP traffic generator */

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <time.h>
#include <sys/time.h>

#define STATE_READY 1
#define STATE_WAITING 2
#define STATE_INACTIVE 3

unsigned int create_task(
  void (*function_ptr)(void *p, unsigned int task_id), 
  void *argument_ptr, unsigned short int state, unsigned int delay
  );
void scheduler();
void halt_me(unsigned int task_id);
void delay(unsigned int usec, unsigned int task_id);

#endif  /* _SERVER_H */