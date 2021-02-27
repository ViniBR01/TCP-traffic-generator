#ifndef _MAIN_H
#define _MAIN_H
#include <stdio.h>

#define TCPTRAFFIC_VERSION "0.1.0"
#define CMD_SERVER   "start-server"
#define CMD_CLIENT "start-client"

typedef struct {
  int           verbose;
  uint32_t      flags;
  FILE         *input;
  FILE         *output;
} options_t;

#define OPTIONS_INIT { stdin, stdout, QUIET }

#endif	/* _MAIN_H */