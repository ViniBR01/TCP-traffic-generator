/* traffic_factory.h - TCP traffic generator */

#ifndef _TRAFFIC_FACTORY_H
#define _TRAFFIC_FACTORY_H

#include <stdint.h>

#define FIXED_PERIODIC 0
//#define POISSON_ARRIVALS 1 //and so on...

typedef struct {
    int id;
    int model_type;
} flow_t;

flow_t* create_flow(int model_type);
int get_next_file();

#endif  /* _TRAFFIC_FACTORY_H */