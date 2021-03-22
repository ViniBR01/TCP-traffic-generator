/* verbosity.h - TCP traffic generator */

#ifndef _VERBOSITY_H
#define _VERBOSITY_H

#include <stdio.h>
#include <stdlib.h>

int set_verbosity(u_int32_t new_level);
int verbosity(char *message, uint32_t message_level);
u_int32_t get_verbosity();

#endif  /* _VERBOSITY_H */