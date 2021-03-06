/* main.c - TCP traffic generator for experiments
Author: Vinicius Da Silva
Date Feb 26 2021

references:
how to write a good main file: https://opensource.com/article/19/5/how-write-good-c-main-function
how to struct multi-file c: https://opensource.com/article/19/7/structure-multi-file-c-part-1
                            https://opensource.com/article/19/7/structure-multi-file-c-part-2
*/

/* 0 copyright/licensing */

/* 1.1 system includes */
//Check /usr/include to learn about the standard C library
#include <stdio.h>  //FILE, stdin, stdout, stderr, and the fprint() family
#include <stdlib.h> //malloc(), calloc(), and realloc()
#include <stdint.h> //special types such as uint32_t
#include <unistd.h> //EXIT_FAILURE, EXIT_SUCCESS
#include <libgen.h> //basename() function
#include <errno.h>  //defines external errno var and the values it can take on
#include <string.h> //memcpy(), memset(), and the strlen() family
#include <getopt.h> //external optarg, opterr, optind, and getopt() function

/* 1.2 project includes */
#include "main.h"
#include "client.h"
#include "server.h"
#include "verbosity.h"

/* 2 defines */
//colon mandates an argument in OPTSTR; W is reserved for long options
#define OPTSTR "v:p:i:t:f:l:m:o:h"
#define USAGE_FMT  "%s [-i remoteIP] [-p port] [-t period_in_ms] [-f filesize_in_kB] [-l length_in_sec] [-m mode_DL/UL] [-o output_file_name] [-v level] [-h]\n"
#define ERR_CLIENT "client function blew up"
#define ERR_SERVER "server function blew up"
#define DEFAULT_PROGNAME "start_client"
#define TCP_OP_INVALID 0
#define TCP_OP_SERVER 1
#define TCP_OP_CLIENT 2

/* 3 external declarations */
extern int errno;
extern char *optarg;
extern int opterr, optind;

/* 4 typedefs */

/* 5 global variable declarations */

/* 6 ancillary function prototypes */
void usage(char *progname, int opt);
int pick_operation(char *);

int main(int argc, char *argv[]) {
  /* Set default verbosity */
  set_verbosity(1);

  /* 7 command-line parsing */
  int tcp_op = TCP_OP_INVALID;

  if ((tcp_op = pick_operation(argv[0])) == TCP_OP_INVALID) {
    errno = EINVAL;
    perror("start-server/start-client argv[0] not recognized.");
    exit(EXIT_FAILURE);
    /* NOTREACHED */
  }

  int opt;
  options_t options = { 8080, "127.0.0.1", 1000, 10, 10, MODE_SINGLE_DL, "download-times.txt" }; //default values

  opterr = 0; //disables getopt from emmiting a ?

  while ((opt = getopt(argc, argv, OPTSTR)) != EOF) {
    switch(opt) {
      
      case 'p':
        options.port = (uint32_t )strtoul(optarg, NULL, 10);
        /* todo: validate the input port number XXX */
        break;
      
      case 'i':
        options.remote_ip = optarg;
        /* todo: validate the input ip number XXX */
        break;
      
      case 't':
        options.period_ms = (uint32_t )strtoul(optarg, NULL, 10);
        /* todo: validate the input period XXX */
        break;
      
      case 'f':
        options.file_size_kb = (uint32_t )strtoul(optarg, NULL, 10);
        /* todo: validate the input file size XXX */
        break;
      
      case 'l':
        options.length_sec = (int )strtol(optarg, NULL, 10);
        /* todo: validate the input file size XXX */
        break;
      
      case 'm':
        options.mode = (uint8_t )strtoul(optarg, NULL, 10);
        /* todo: validate the input file size XXX */
        break;

      case 'o':
        options.output_name = optarg;
        /* todo: validate the input file size XXX */
        break;

      case 'v':
        set_verbosity((uint32_t )strtoul(optarg, NULL, 10));
        break;

      case 'h':
        fprintf(stderr, USAGE_FMT, basename(argv[0]));
        exit(EXIT_SUCCESS);
        /* NOTREACHED */
      default:
        usage(basename(argv[0]), opt);
        /* NOTREACHED */
        break;
    }
  }

  switch(tcp_op) {
    case TCP_OP_CLIENT:
      /* CALL THE FUNCTIONS RELATED TO CLIENT EXECUTION XXX */
      verbosity("Entering client mode operation.\n", 1);

      if (client(&options) != EXIT_SUCCESS) {
        perror(ERR_CLIENT);
        exit(EXIT_FAILURE);
        /* NOTREACHED */
      }

      break;
    
    case TCP_OP_SERVER:
      /* CALL THE FUNCTIONS RELATED TO SERVER EXECUTION XXX */
      verbosity("Entering server mode operation.\n", 1);

      if (server(&options) != EXIT_SUCCESS) {
        perror(ERR_SERVER);
        exit(EXIT_FAILURE);
        /* NOTREACHED */
      }

      break;
    
    default:
      errno = EINVAL;
      fprintf(stderr, "Impossible mode: %s tcp_op=%d\n", argv[0], tcp_op);
      break;
  }

  return EXIT_SUCCESS;
}

/* 8 ancillary function declarations */
void usage(char *progname, int opt) {
  if (!progname) {
    errno = EINVAL;
    perror("main:usage called with NULL argv[0]");
    exit(EXIT_FAILURE);
  }
  
  fprintf(stderr, USAGE_FMT, basename(progname));
  if (opt != '?') {
    fprintf(stderr, "unknown option: \"%c\"\n", opt);
  }
  exit(EXIT_FAILURE);
  /* NOTREACHED */
}

int pick_operation(char *argv0)
{
  char *name;
  
  if (!argv0) {
    errno = EINVAL;
    return TCP_OP_INVALID;
  }
  
  name = basename(argv0);

  if (strncmp(name, CMD_SERVER, strlen(CMD_SERVER)) == 0)
    return TCP_OP_SERVER;

  if (strncmp(name, CMD_CLIENT, strlen(CMD_CLIENT)) == 0) 
    return TCP_OP_CLIENT;  
  
  return TCP_OP_INVALID;
}