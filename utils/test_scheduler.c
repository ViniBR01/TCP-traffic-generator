/* Script to test the task scheduler implementation */

#include <stdio.h>
#include <stdlib.h>

#include "scheduler.h"

void periodic_task(void *p);

int main() {
    printf("Starting test: task scheduler.\n");
    /* Test concurrency in tasks that use the same fuction

    //Should be able to create a simple periodic task and run the scheduler
    int j = 0;
    TaskList[j].ftpr = periodic_task;
    unsigned int *task_parameter = (unsigned int *) malloc(sizeof(unsigned int));
    *task_parameter = 1000000;
    TaskList[j].arg_ptr = (void *) task_parameter;
    TaskList[j].state = STATE_READY;
    TaskList[j].delay = -1;

    j++;
    TaskList[j].ftpr = periodic_task;
    unsigned int *task_parameter2 = (unsigned int *) malloc(sizeof(unsigned int));
    *task_parameter2 = 3000000;
    TaskList[j].arg_ptr = (void *) task_parameter2;
    TaskList[j].state = STATE_READY;
    TaskList[j].delay = -1;

    j++;
    TaskList[j].ftpr = NULL; */

    unsigned int *task_parameter1 = (unsigned int *) malloc(sizeof(unsigned int));
    *task_parameter1 = 1000000;
    create_task(periodic_task, (void *)task_parameter1, STATE_READY, -1);

    unsigned int *task_parameter2 = (unsigned int *) malloc(sizeof(unsigned int));
    *task_parameter2 = 3000000;
    create_task(periodic_task, (void *)task_parameter2, STATE_READY, -1);

    

    //Now start scheduler
    task_index = 0;
    while(1) {
        scheduler();
    }

    return 0;
}

void periodic_task(void *p) {
    printf("Executing the periodic task with id: %d.\n", task_index);
    printf("%d\n", *(unsigned int *)p);
    // printf("pointer address: %p\n\n", *(unsigned int *)p);
    delay(*(unsigned int *)p);
    return;
}