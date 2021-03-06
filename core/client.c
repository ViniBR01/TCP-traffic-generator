/* client.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "client.h"
#include "scheduler.h"
#include "open_server.h"
#include "verbosity.h"

int client(options_t *options) {
    verbosity("Inside of the client function.\n", 3);

    if (!options) {
        errno = EINVAL;
        return EXIT_FAILURE;
    }

    /* XXX do needful stuff */
    server_t *server_options = setup_server(options->port);
    create_task(check_connections, (void *) server_options, STATE_READY, -1);

    /* Now start scheduler */
    while(1) {
        scheduler();
    }

    return EXIT_SUCCESS;
}