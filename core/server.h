/* server.h - TCP traffic generator */

#ifndef _SERVER_H
#define _SERVER_H

#include <stdint.h>
#include <stdlib.h>

#define MODE_SINGLE_DL 1
#define MODE_SINGLE_UL 2

typedef struct {
  int           verbose;
  uint32_t      port;
  char         *remote_ip;
  uint32_t      period_ms;
  uint32_t      file_size_kb;
  int           length_sec;
  uint32_t      mode;
  char         *output_name;
} options_t;

int server(options_t *options);

#endif  /* _SERVER_H */