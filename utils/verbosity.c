/* verbosity.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "verbosity.h"

static u_int32_t level;

int set_verbosity(u_int32_t new_level) {
    level = new_level;
    return EXIT_SUCCESS;
}

int verbosity(char *message, u_int32_t message_level) {
    if (level >= message_level) {
        printf(message);
    }
    return EXIT_SUCCESS;
}

int verbosity_int(char *message, int parameter1, u_int32_t message_level) {
    if (level >= message_level) {
        printf(message, parameter1);
    }
    return EXIT_SUCCESS;
}

u_int32_t get_verbosity() {
    return level;
}