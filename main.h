#ifndef _MAIN_H
#define _MAIN_H
#include <stdio.h>

#define TCPTRAFFIC_VERSION "0.1.0"
#define CMD_SERVER   "start-server"
#define CMD_CLIENT "start-client"

typedef struct {
  int           verbose;
  //uint32_t      flags;
  //FILE       *configs;
  uint32_t      port;
  char         *remote_ip;
  uint32_t      period;
  uint32_t      file_size;
} options_t;

#endif	/* _MAIN_H */