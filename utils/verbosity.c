/* verbosity.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "verbosity.h"
#include "scheduler.h"

static u_int32_t level;

int set_verbosity(u_int32_t new_level) {
    level = new_level;
    return EXIT_SUCCESS;
}

int verbosity(char *message, u_int32_t message_level) {
    if (level >= message_level) {
        if (level >= 4) {
            printf("t=%10d\t", get_scheduler_time_usec());
        }
        printf(message);
    }
    return EXIT_SUCCESS;
}

int verbosity_int(char *message, int parameter1, u_int32_t message_level) {
    if (level >= message_level) {
        if (level >= 4) {
            printf("t=%10d\t", get_scheduler_time_usec());
        }
        printf(message, parameter1);
    }
    return EXIT_SUCCESS;
}

int verbosity_string(char *message, char *parameter1, u_int32_t message_level) {
    if (level >= message_level) {
        if (level >= 4) {
            printf("t=%10d\t", get_scheduler_time_usec());
        }
        printf(message, parameter1);
    }
    return EXIT_SUCCESS;
}

u_int32_t get_verbosity() {
    return level;
}