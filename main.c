/* main.c - TCP traffic generator for experiments
Author: Vinicius Da Silva
Date Feb 26 2021

references:
how to write a good main file: https://opensource.com/article/19/5/how-write-good-c-main-function
how to struct multi-file c: https://opensource.com/article/19/7/structure-multi-file-c-part-1
                            https://opensource.com/article/19/7/structure-multi-file-c-part-2
*/

/* 0 copyright/licensing */
//to-do XXX

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

/* 2 defines */
//colon mandates an argument in OPTSTR; W is reserved for long options
#define OPTSTR "vi:o:f:h"
#define USAGE_FMT  "%s [-v] [-f hexflag] [-i inputfile] [-o outputfile] [-h]\n"
#define ERR_FOPEN_INPUT  "fopen(input, r)"
#define ERR_FOPEN_OUTPUT "fopen(output, w)"
#define ERR_DO_THE_NEEDFUL "do_the_needful blew up"
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

/* 6 function prototypes */
void usage(char *progname, int opt);
int pick_operation(char *);
int do_the_needful(options_t *options);

int main(int argc, char *argv[]) {
  /* 7 command-line parsing */
  int opt;
  options_t options = { 0, 0x0, stdin, stdout }; //default values

  opterr = 0; //disables getopt from emmiting a ?

  while ((opt = getopt(argc, argv, OPTSTR)) != EOF)
    switch(opt) {
    case 'i':
      if (!(options.input = fopen(optarg, "r")) ){
        perror(ERR_FOPEN_INPUT);
        exit(EXIT_FAILURE);
        /* NOTREACHED */
      }
      break;

    case 'o':
      if (!(options.output = fopen(optarg, "w")) ){
        perror(ERR_FOPEN_OUTPUT);
        exit(EXIT_FAILURE);
        /* NOTREACHED */
      }    
      break;
             
    case 'f':
      options.flags = (uint32_t )strtoul(optarg, NULL, 16);
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

  if (do_the_needful(&options) != EXIT_SUCCESS) {
    perror(ERR_DO_THE_NEEDFUL);
    exit(EXIT_FAILURE);
    /* NOTREACHED */
  }

  return EXIT_SUCCESS;
}

/* 8 function declarations */
void usage(char *progname, int opt) {
  if (!argv0) {
    errno = EINVAL;
    perror("main:usage called with NULL argv[0]");
    exit(-1);
  }
  
  fprintf(stderr, "usage: %s [-i input] [-o output] [-V]\n",
	  basename(argv0));
  if (opt != '?')
    fprintf(stderr, "unknown option: \"%c\"\n", opt);
  exit(-1);
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

int do_the_needful(options_t *options) {

  if (!options) {
    errno = EINVAL;
    return EXIT_FAILURE;
  }

  if (!options->input || !options->output) {
    errno = ENOENT;
    return EXIT_FAILURE;
  }

  /* XXX do needful stuff */

  return EXIT_SUCCESS;
}
