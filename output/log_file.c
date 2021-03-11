/* log_file.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>

#include "log_file.h"

int log_int(int val) {

    FILE *fp;
    fp = fopen("output/download-times.txt", "a+");
    if (fp == NULL) {
        perror("error opening file");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    } else {
        fprintf(fp, "%d\n", val);
        fclose(fp);
    }

    return (EXIT_SUCCESS);
}