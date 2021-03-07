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

#define MIN_TCB_SIZE 10
#define ERR_INVALID_STATE "The task state was invalid"
#define NOT_ITERATING -1

typedef struct TASKstruct {
  unsigned int task_id;
  void (*function_ptr)(void *p, unsigned int task_id);
  void *arg_ptr;
  unsigned short int state;
  unsigned int delay;
  struct timeval delay_ref;	  //reference time when delay started
} task_t;

typedef struct TCBstruct {
  task_t *task_list;
  int capacity;
  int count;
  int iterator;
} tcb_t;

// unsigned int task_index = 0;
unsigned int creating_id = 0;
tcb_t ready_tasks;
tcb_t halted_tasks;
tcb_t sleeping_tasks;

/* In order to store the tasks we need a data structure. As a design choice, 
we will use three copies of this DS to store active, sleeping and halted tasks.
  This DS must provide the following functionalities:
  -Add new task with a give id number
  -Delete a task given an id number
  -Check if a given id number is present
  -Return a task with a give id number, if it exists
  -Allow arbitrary number of tasks
  -Efficient iteration of all tasks in the DS
  -It must be independent of the actual values of the id numbers (they are only unique)

Ideally, we would like to have a hashtable for best bigO time performance.
However, for a simpler implementation in C, we will use simple arrays.
*/

int add_task(unsigned int new_task_id,
              void (*new_function_ptr)(void *p, unsigned int task_id),
              void *new_arg_ptr, unsigned short int new_state, 
              unsigned int new_delay, tcb_t task_block) {
  if (task_block.task_list == NULL) {
    task_block.task_list = (task_t *) malloc(MIN_TCB_SIZE * sizeof(task_t));
    task_block.capacity = MIN_TCB_SIZE;
    task_block.count = 0;
    task_block.iterator = NOT_ITERATING;
  } else if (task_block.count == task_block.capacity) {
    increase_task_block(task_block); // XXX write this fuction later
  }

  /* Here we guarantee that the task block is allocated and has free space */
  int new_index = task_block.count;
  task_block.task_list[new_index].task_id = new_task_id;
  task_block.task_list[new_index].function_ptr = new_function_ptr;
  task_block.task_list[new_index].arg_ptr = new_arg_ptr;
  task_block.task_list[new_index].state = new_state;
  task_block.task_list[new_index].delay = new_delay;
  gettimeofday(&(task_block.task_list[new_index].delay_ref), NULL);
  task_block.count += 1;
}

task_t * get_task(unsigned int task_id, tcb_t task_block) {
  if (task_block.task_list == NULL) {
    return NULL;
  }

  for(int i=0; i<task_block.count; i++) {
    if(task_block.task_list[i].task_id == task_id) {
      return task_block.task_list+i;
    }
  }

  return NULL;
}

int delete_task(unsigned int delete_id, tcb_t task_block) {
  /* This function assumes a task is present in the given task block */

  int delete_index = 0;
  while(task_block.task_list[delete_index].task_id != delete_id) {
    delete_id += 1;
  }

  for(int i=delete_task; i<task_block.count-1; i++) {
    task_block.task_list[i].task_id = task_block.task_list[i+1].task_id;
    task_block.task_list[i].function_ptr = task_block.task_list[i+1].function_ptr;
    task_block.task_list[i].arg_ptr = task_block.task_list[i+1].arg_ptr;
    task_block.task_list[i].state = task_block.task_list[i+1].state;
    task_block.task_list[i].delay = task_block.task_list[i+1].delay;
    task_block.task_list[i].delay_ref = task_block.task_list[i+1].delay_ref;
  }

  task_block.count -= 1;
  
  if (task_block.iterator > delete_id) {
    task_block.iterator -= 1;
  } else if (task_block.iterator == task_block.count) {
    task_block.iterator = NOT_ITERATING;
  }
}

int start_iterator(tcb_t task_block) {
  if (task_block.task_list != NULL) {
    task_block.iterator = 0;
  } else {
    task_block.iterator = NOT_ITERATING;
  }
  return task_block.iterator;
}

int advance_iterator(tcb_t task_block) {
  if (task_block.task_list != NULL && task_block.iterator < task_block.count-1) {
    task_block.iterator += 1;
  } else {
    task_block.iterator = NOT_ITERATING;
  }
  return task_block.iterator;
}
/* end of the task block control Data Structure definition */

static void start_task(void (*function_ptr)(void *, unsigned int) , void* param_ptr, unsigned int task_id){
  function_ptr(param_ptr, task_id);
}

unsigned int create_task( void (*function_ptr)(void *p, unsigned int task_id), \
                          void *argument_ptr, unsigned short int state, unsigned int delay) {
  int task_id = creating_id;
  creating_id += 1;
  switch(state) {
    case STATE_READY:
    add_task(task_id, function_ptr, argument_ptr, state, delay, ready_tasks);
    break;

    case STATE_WAITING:
    add_task(task_id, function_ptr, argument_ptr, state, delay, sleeping_tasks);
    break;

    case STATE_INACTIVE:
    add_task(task_id, function_ptr, argument_ptr, state, delay, halted_tasks);
    break;

    default:
    perror(ERR_INVALID_STATE);
    exit(EXIT_FAILURE);
    /* NOTREACHED */
  }
}

void scheduler(){
  /* Check on all sleeping tasks and move them to ready task if delay is up */

  //CARE: the 3 tcb DS may change during the loop execution!!!

  if(sleeping_tasks.task_list != NULL) {
    int i = start_iterator(sleeping_tasks);
    while(i != NOT_ITERATING) {
      //Here we know that a task exists at position i of sleeping tasks
      //check if the timer is up
      //if yes, move it to ready tasks
      //if not, dont do anything
      i = advance_iterator(sleeping_tasks);
    }
  }

  /* Iterate through all ready tasks and execute them */
  if(ready_tasks.task_list != NULL) {
    for(int i=0; i<ready_tasks.count; i++) {

    }
  }







  if(TaskList[task_index].function_ptr == NULL && task_index != 0) {
    task_index = 0;
  }
  //if(TaskList[task_index].ftpr == NULL && task_index == 0) {
  //	printf("No tasks exist.\n");/*No tasks!*/
  if(TaskList[task_index].state == STATE_READY) {
    start_task(TaskList[task_index].function_ptr, TaskList[task_index].arg_ptr, task_index);
  }else if(TaskList[task_index].state == STATE_WAITING) {
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
  /* Deactivates one task indicated by task_id */

  task_t *task = get_task(task_id, ready_tasks);
  if((task) != NULL) {
    add_task(task_id, task->function_ptr, task->arg_ptr, task->state, task->delay, halted_tasks);
    delete_task(task_id, ready_tasks);
    return;
  }

  task = get_task(task_id, sleeping_tasks);
  if((task) != NULL) {
    add_task(task_id, task->function_ptr, task->arg_ptr, task->state, task->delay, halted_tasks);
    delete_task(task_id, sleeping_tasks);
    return;
  }
}

void delay(unsigned int new_delay, unsigned int task_id){
  /* Updates the delay value and delay ref of one task indicated by task_id.
     If the task was not in the sleeping list, move it there. */

  task_t *task = get_task(task_id, ready_tasks);
  if((task) != NULL) {
    add_task(task_id, task->function_ptr, task->arg_ptr, task->state, new_delay, sleeping_tasks);
    delete_task(task_id, ready_tasks);
    return;
  }

  task = get_task(task_id, halted_tasks);
  if((task) != NULL) {
    add_task(task_id, task->function_ptr, task->arg_ptr, task->state, new_delay, sleeping_tasks);
    delete_task(task_id, halted_tasks);
    return;
  }

  task = get_task(task_id, sleeping_tasks);
  if((task) != NULL) {
    task->delay = new_delay;
    gettimeofday(&(task->delay_ref), NULL);
    return;
  }
}