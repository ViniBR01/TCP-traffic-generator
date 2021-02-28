/* client.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "client.h"

int client(options_t *options) {
    printf("Inside of the client function.\n");

    if (!options) {
        errno = EINVAL;
        return EXIT_FAILURE;
    }

    /* XXX do needful stuff */
    
    return EXIT_SUCCESS;
}