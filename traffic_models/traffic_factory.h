/* traffic_factory.h - TCP traffic generator */

#ifndef _TRAFFIC_FACTORY_H
#define _TRAFFIC_FACTORY_H

#include <stdio.h>

#define FIXED_PERIODIC 0
//#define POISSON_ARRIVALS 1 //and so on...

typedef struct {
    int id;
    int model_type;
} flow_t;

flow_t* create_flow(int model_type);
int get_next_file();
//More operations will be defined based on the design of the main scheduler

#endif  /* _TRAFFIC_FACTORY_H */

/*
XXX - first implement a single concrete traffic model only.
Later, use the factory design pattern to make it flexible
with more traffic models.

reference: https://stackoverflow.com/questions/3204511/factory-pattern-implementation-using-ansi-c
*/