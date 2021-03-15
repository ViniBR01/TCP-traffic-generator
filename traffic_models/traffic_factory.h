/* traffic_factory.h - TCP traffic generator */

#ifndef _TRAFFIC_FACTORY_H
#define _TRAFFIC_FACTORY_H

#include <stdio.h>

#define FIXED_PERIODIC 0
//#define POISSON_ARRIVALS 1 //and so on...

typedef struct {
    int id;
    int model_type;
    int execution_time_sec;
    uint32_t period_ms;
    uint32_t file_size_kb;
    char *remote_ip;
    uint32_t port;
} traffic_t;

int create_traffic(traffic_t* traffic_parameters);

#endif  /* _TRAFFIC_FACTORY_H */

/*
XXX - first implement a single concrete traffic model only.
Later, use the factory design pattern to make it flexible
with more traffic models.

reference: https://stackoverflow.com/questions/3204511/factory-pattern-implementation-using-ansi-c
*/