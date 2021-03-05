/* traffic_factory.c - TCP traffic generator */

#include <stdio.h>  //FILE, stdin, stdout, stderr, and the fprint() family
#include <stdlib.h> //malloc(), calloc(), and realloc()
#include <stdint.h> //special types such as uint32_t
#include <unistd.h> //EXIT_FAILURE, EXIT_SUCCESS
#include <libgen.h> //basename() function
#include <errno.h>  //defines external errno var and the values it can take on
#include <string.h> //memcpy(), memset(), and the strlen() family
#include <getopt.h> //external optarg, opterr, optind, and getopt() function

#include "traffic_factory.h"
#include "m_fixed_periodic.h"

flow_t* create_flow(int model_type) {
    return NULL;
}