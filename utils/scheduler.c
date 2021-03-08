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

static unsigned int creating_id = 0;
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

void increase_task_block(tcb_t *task_block) {
  // XXX write this fuction later
  return;
}

int add_task(unsigned int new_task_id,
              void (*new_function_ptr)(void *p, unsigned int task_id),
              void *new_arg_ptr, unsigned short int new_state, 
              int new_delay, tcb_t *task_block) {
  
  //printf("\tAdd a new task to the DS: taks_id=%d\n", new_task_id);
  
  if (task_block->task_list == NULL) {

    //printf("\t\tThe task block was not initiated. Use malloc to start it.\n");
    //printf("\t\tPointer value before malloc: %p\n", task_block->task_list);

    task_block->task_list = (task_t *) malloc(MIN_TCB_SIZE * sizeof(task_t));

    //printf("\t\tPointer value after malloc: %p\n", task_block->task_list);

    task_block->capacity = MIN_TCB_SIZE;
    task_block->count = 0;
    task_block->iterator = NOT_ITERATING;
  } else if (task_block->count == task_block->capacity) {
    increase_task_block(task_block);
  }

  /* Here we guarantee that the task block is allocated and has free space */
  int new_index = task_block->count;
  task_block->task_list[new_index].task_id = new_task_id;
  task_block->task_list[new_index].function_ptr = new_function_ptr;
  task_block->task_list[new_index].arg_ptr = new_arg_ptr;
  task_block->task_list[new_index].state = new_state;
  task_block->task_list[new_index].delay = new_delay;
  gettimeofday(&(task_block->task_list[new_index].delay_ref), NULL);
  task_block->count += 1;

  //printf("\t\tReady task counter = %d\n", ready_tasks.count);

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

int delete_task(unsigned int delete_id, tcb_t *task_block) {
  /* This function assumes a task is present in the given task block */

  if (task_block->iterator >= delete_id) {
    task_block->iterator -= 1;
  }

  int delete_index = 0;
  while(task_block->task_list[delete_index].task_id != delete_id) {
    delete_index += 1;
  }

  for(int i=delete_index; i<task_block->count-1; i++) {
    task_block->task_list[i].task_id = task_block->task_list[i+1].task_id;
    task_block->task_list[i].function_ptr = task_block->task_list[i+1].function_ptr;
    task_block->task_list[i].arg_ptr = task_block->task_list[i+1].arg_ptr;
    task_block->task_list[i].state = task_block->task_list[i+1].state;
    task_block->task_list[i].delay = task_block->task_list[i+1].delay;
    task_block->task_list[i].delay_ref = task_block->task_list[i+1].delay_ref;
  }

  task_block->count -= 1;

}

int start_iterator(tcb_t *task_block) {
  if (task_block->task_list != NULL) {
    task_block->iterator = 0;
  } else {
    task_block->iterator = NOT_ITERATING;
  }
  return task_block->iterator;
}

int advance_iterator(tcb_t *task_block) {
  if (task_block->task_list != NULL && task_block->iterator < task_block->count-1) {
    task_block->iterator += 1;
  } else {
    task_block->iterator = NOT_ITERATING;
  }
  return task_block->iterator;
}
/* end of the task block control Data Structure definition */
/////////////////////////////////////////////////////////////

static void start_task(void (*function_ptr)(void *, unsigned int) , void* param_ptr, unsigned int task_id){
  function_ptr(param_ptr, task_id);
}

unsigned int create_task( void (*function_ptr)(void *p, unsigned int task_id), \
                          void *argument_ptr, unsigned short int state, unsigned int delay) {
  
  //printf("Creating a new task: taks_id=%d\n", creating_id);

  //printf("\tReady task counter = %d\n", ready_tasks.count);
  
  int task_id = creating_id;
  creating_id += 1;
  switch(state) {
    case STATE_READY:
    //printf("\tThe task state was set to STATE_READY.\n");
    add_task(task_id, function_ptr, argument_ptr, state, delay, &ready_tasks);
    break;

    case STATE_WAITING:
    add_task(task_id, function_ptr, argument_ptr, state, delay, &sleeping_tasks);
    break;

    case STATE_INACTIVE:
    add_task(task_id, function_ptr, argument_ptr, state, delay, &halted_tasks);
    break;

    default:
    perror(ERR_INVALID_STATE);
    exit(EXIT_FAILURE);
    /* NOTREACHED */
  }

  //printf("\tReady task counter = %d\n", ready_tasks.count);

}

int kill_task(unsigned int task_id) {
  return 0;
}

void scheduler(){
  /* Check on all sleeping tasks and move them to ready task if delay is up */

  /* CARE: the 3 tcb DS may change during the loop execution!!! 
    Must use built-in iterator*/

  ////printf("Start another round of the scheduler.\n");

  if(sleeping_tasks.task_list != NULL && sleeping_tasks.count > 0) {
    struct timeval elapsedTime;
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    int i = start_iterator(&sleeping_tasks);
    
    while(i != NOT_ITERATING) {
      timersub(&currentTime, &(sleeping_tasks.task_list[i].delay_ref), &elapsedTime);
      unsigned long time_in_micros = 1000000*elapsedTime.tv_sec + elapsedTime.tv_usec;

      if (time_in_micros >= sleeping_tasks.task_list[i].delay) {
        task_t *task = get_task(sleeping_tasks.task_list[i].task_id, sleeping_tasks);
        add_task(task->task_id, task->function_ptr, task->arg_ptr, task->state, task->delay, &ready_tasks);
        delete_task(task->task_id, &sleeping_tasks);
      }

      i = advance_iterator(&sleeping_tasks);
    }
  }

  /* Iterate through all ready tasks and execute them */
  if(ready_tasks.task_list != NULL && ready_tasks.count > 0) {

    //printf("The struct ready_tasks was already initiated.\n");

    int i = start_iterator(&ready_tasks);
    
    while(i != NOT_ITERATING) {
      task_t *task = get_task(ready_tasks.task_list[i].task_id, ready_tasks);
      start_task(task->function_ptr, task->arg_ptr, task->task_id);

      i = advance_iterator(&ready_tasks);
    }
  }

  return;
}

void halt_me(unsigned int task_id){
  /* Deactivates one task indicated by task_id */

  task_t *task = get_task(task_id, ready_tasks);
  if((task) != NULL) {
    add_task(task_id, task->function_ptr, task->arg_ptr, task->state, task->delay, &halted_tasks);
    delete_task(task_id, &ready_tasks);
    return;
  }

  task = get_task(task_id, sleeping_tasks);
  if((task) != NULL) {
    add_task(task_id, task->function_ptr, task->arg_ptr, task->state, task->delay, &halted_tasks);
    delete_task(task_id, &sleeping_tasks);
    return;
  }
}

void delay(int new_delay, unsigned int task_id){
  /* Updates the delay value and delay ref of one task indicated by task_id.
     If the task was not in the sleeping list, move it there. */

  task_t *task = get_task(task_id, ready_tasks);
  if((task) != NULL) {
    add_task(task_id, task->function_ptr, task->arg_ptr, task->state, new_delay, &sleeping_tasks);
    delete_task(task_id, &ready_tasks);
    return;
  }

  task = get_task(task_id, halted_tasks);
  if((task) != NULL) {
    add_task(task_id, task->function_ptr, task->arg_ptr, task->state, new_delay, &sleeping_tasks);
    delete_task(task_id, &halted_tasks);
    return;
  }

  task = get_task(task_id, sleeping_tasks);
  if((task) != NULL) {
    task->delay = new_delay;
    gettimeofday(&(task->delay_ref), NULL);
    return;
  }
}