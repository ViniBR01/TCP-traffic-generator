/* Script to test the task scheduler implementation */

#include <stdio.h>
#include <stdlib.h>

#include "scheduler.h"

void periodic_task(void *p, unsigned int task_id);
void task_to_kill_task(void *p, unsigned int task_id);

int main() {
    printf("Starting test: task scheduler.\n");
    /* Test concurrency in tasks that use the same fuction */

    unsigned int *task_parameter1 = (unsigned int *) malloc(sizeof(unsigned int));
    *task_parameter1 = 3000000;
    create_task(periodic_task, (void *)task_parameter1, STATE_READY, -1);

    unsigned int *task_parameter2 = (unsigned int *) malloc(sizeof(unsigned int));
    *task_parameter2 = 1000000;
    unsigned int my_id = create_task(periodic_task, (void *)task_parameter2, STATE_READY, -1);

    unsigned int *task_parameter3 = (unsigned int *) malloc(sizeof(unsigned int));
    *task_parameter3 = my_id;
    create_task(task_to_kill_task, (void *)task_parameter3, STATE_WAITING, 5500000);

    //Now start scheduler
    while(1) {
        scheduler();
    }

    return 0;
}

void periodic_task(void *p, unsigned int task_id) {
    printf("Executing the periodic task with id: %d.\n", task_id);
    static int counter = 0;
    counter++;
    printf("\tparam_value=%d | counter=%d\n", *(unsigned int *)p, counter);
    // printf("pointer address: %p\n\n", *(unsigned int *)p);
    delay(*(unsigned int *)p, task_id);
    return;
}

void task_to_kill_task(void *p, unsigned int task_id) {
    printf("Executing the task to kill one task. My id: %d.\n", task_id);
    printf("\tkill task with id: %d\n", *(unsigned int *)p);
    kill_task(*(unsigned int *)p);
    halt_me(task_id);
}