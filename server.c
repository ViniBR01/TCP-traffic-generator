/* server.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "server.h"

int server(options_t *options) {
    printf("Inside of the server function.\n");

    if (!options) {
        errno = EINVAL;
        return EXIT_FAILURE;
    }

    /* XXX do server stuff */

    return EXIT_SUCCESS;
}