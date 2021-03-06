/* Script to test the task scheduler implementation */

#include <stdio.h>
#include <stdlib.h>

#include "scheduler.h"

void periodic_task(void *p);

int main() {
    printf("Starting test: task scheduler.\n");
    //Should be able to create a simple periodic task and run the scheduler
    int j = 0;
    TaskList[j].ftpr = periodic_task;
    unsigned int period1 = 100; //not working with local variable. Try malloc
    unsigned int *task_parameter = &period1;
    task_parameter = (unsigned int *) malloc(sizeof(unsigned int));
    *task_parameter = 1000000;
    TaskList[j].arg_ptr = (void *) task_parameter;
    TaskList[j].state = STATE_READY;
    TaskList[j].delay = -1;

    j++;
    TaskList[j].ftpr = NULL;

    // printf("%d\n", period1);
    // printf("%d\n", *task_parameter);
    // printf("%d\n", *(unsigned int *)TaskList[0].arg_ptr);
    // printf("pointer address: %p\n", task_parameter);
    // printf("pointer address: %p\n\n", (unsigned int *)TaskList[0].arg_ptr);
    

    //Now start scheduler
    task_index = 0;
    while(1) {
        scheduler();
    }

    return 0;
}

void periodic_task(void *p) {
    task_index = 0;
    printf("Executing the periodic task.\n");
    printf("%d\n", *(unsigned int *)p);
    // printf("pointer address: %p\n\n", *(unsigned int *)p);
    delay(*(unsigned int *)p);
    return;
}