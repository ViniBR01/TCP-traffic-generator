/* Script to test the task scheduler implementation */

#include <stdio.h>
#include <stdlib.h>

#include "scheduler.h"

void periodic_task(void *p, unsigned int task_id);

int main() {
    printf("Starting test: task scheduler.\n");
    /* Test concurrency in tasks that use the same fuction */

    unsigned int *task_parameter1 = (unsigned int *) malloc(sizeof(unsigned int));
    *task_parameter1 = 1000000;
    create_task(periodic_task, (void *)task_parameter1, STATE_READY, -1);

    unsigned int *task_parameter2 = (unsigned int *) malloc(sizeof(unsigned int));
    *task_parameter2 = 3000000;
    create_task(periodic_task, (void *)task_parameter2, STATE_READY, -1);

    

    //Now start scheduler
    while(1) {
        scheduler();
    }

    return 0;
}

void periodic_task(void *p, unsigned int task_id) {
    printf("Executing the periodic task with id: %d.\n", task_id);
    printf("%d\n", *(unsigned int *)p);
    // printf("pointer address: %p\n\n", *(unsigned int *)p);
    delay(*(unsigned int *)p, task_id);
    return;
}