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

/* 2 defines */
//colon mandates an argument in OPTSTR; W is reserved for long options
#define OPTSTR "vp:i:t:f:h"
#define USAGE_FMT  "%s [-v] [-f hexflag] [-i inputfile] [-o outputfile] [-h]\n"
//#define ERR_FOPEN_INPUT  "fopen(input, r) XXX"
//#define ERR_FOPEN_OUTPUT "fopen(output, w) XXX"
#define ERR_CLIENT "client function blew up"
#define ERR_SERVER "server function blew up"
#define DEFAULT_PROGNAME "start_server"
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
int do_the_needful(options_t *options);

int main(int argc, char *argv[]) {
  /* 7 command-line parsing */
  int tcp_op = TCP_OP_INVALID;

  if ((tcp_op = pick_operation(argv[0])) == TCP_OP_INVALID) {
    errno = EINVAL;
    perror("start-server/start-client argv[0] not recognized.");
    exit(EXIT_FAILURE);
    /* NOTREACHED */
  }

  int opt;
  options_t options = { 0, 8080, "", 1000, 10 }; //default values

  opterr = 0; //disables getopt from emmiting a ?

  while ((opt = getopt(argc, argv, OPTSTR)) != EOF) {
    switch(opt) {
      //case 'c':
        //   if (!(options.configs = fopen(optarg, "r")) ){
        //     perror(ERR_FOPEN_INPUT);
        //     exit(EXIT_FAILURE);
        //     /* NOTREACHED */
        //   }
        //break
      
      case 'p':
        options.port = (uint32_t )strtoul(optarg, NULL, 16);
        /* todo: validate the input port number XXX */
        break;
      
      case 'i':
        options.remote_ip = optarg;
        /* todo: validate the input ip number XXX */
        break;
      
      case 't':
        options.period = (uint32_t )strtoul(optarg, NULL, 16);
        /* todo: validate the input period XXX */
        break;
      
      case 'f':
        options.file_size = (uint32_t )strtoul(optarg, NULL, 16);
        /* todo: validate the input file size XXX */
        break;

      case 'v':
        options.verbose += 1;
        break;

      case 'h':
      default:
        usage(basename(argv[0]), opt);
        /* NOTREACHED */
        break;
    }
  }

  switch(tcp_op) {
    case TCP_OP_CLIENT:
      /* CALL THE FUNCTIONS RELATED TO CLIENT EXECUTION XXX */
      printf("Entering client mode operation.\n");

      if (client(&options) != EXIT_SUCCESS) {
        perror(ERR_CLIENT);
        exit(EXIT_FAILURE);
        /* NOTREACHED */
      }

      break;
    
    case TCP_OP_SERVER:
      /* CALL THE FUNCTIONS RELATED TO SERVER EXECUTION XXX */
      printf("Entering server mode operation.\n");

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
  
  fprintf(stderr, "usage: %s [-i input] [-o output] [-V]\n", basename(progname));
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