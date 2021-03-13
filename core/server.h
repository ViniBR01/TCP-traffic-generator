/* server.h - TCP traffic generator */

#ifndef _SERVER_H
#define _SERVER_H

#include <stdint.h>

typedef struct {
  int           verbose;
  //uint32_t    flags;
  //FILE       *configs;
  uint32_t      port;
  char         *remote_ip;
  uint32_t      period_ms;
  uint32_t      file_size_kb;
} options_t;

int server(options_t *options);

#endif  /* _SERVER_H */