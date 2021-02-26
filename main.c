/* main.c - the complete listing */

/* 0 copyright/licensing */
//to-do XXX

/* 1 includes */
//Check /usr/include to learn about the standard C library
#include <stdio.h>  //FILE, stdin, stdout, stderr, and the fprint()
#include <stdlib.h> //malloc(), calloc(), and realloc()
#include <stdint.h> //special types such as uint32_t
#include <unistd.h> //EXIT_FAILURE, EXIT_SUCCESS
#include <libgen.h> //basename() function
#include <errno.h>  //defines external errno var and the values it can take on
#include <string.h> //memcpy(), memset(), and the strlen() family
#include <getopt.h> //external optarg, opterr, optind, and getopt() function


/* 2 defines */
#define OPTSTR "vi:o:f:h"
#define USAGE_FMT  "%s [-v] [-f hexflag] [-i inputfile] [-o outputfile] [-h]"
#define ERR_FOPEN_INPUT  "fopen(input, r)"
#define ERR_FOPEN_OUTPUT "fopen(output, w)"
#define ERR_DO_THE_NEEDFUL "do_the_needful blew up"
#define DEFAULT_PROGNAME "george"

/* 3 external declarations */
extern int errno;
extern char *optarg;
extern int opterr, optind;

/* 4 typedefs */
typedef struct {
  int           verbose;
  uint32_t      flags;
  FILE         *input;
  FILE         *output;
} options_t;

/* 5 global variable declarations */
int dumb_global_variable = -11;

/* 6 function prototypes */
void usage(char *progname, int opt);
int  do_the_needful(options_t *options);

int main(int argc, char *argv[]) {
  /* 7 command-line parsing */
  int opt;
  options_t options = { 0, 0x0, stdin, stdout };

  opterr = 0;

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
  fprintf(stderr, USAGE_FMT, progname?progname:DEFAULT_PROGNAME);
  exit(EXIT_FAILURE);
  /* NOTREACHED */
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
