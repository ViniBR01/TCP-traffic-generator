/* log_file.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log_file.h"
#include "verbosity.h"

char* itoa(int value, char* result, int base);

int log_int(int val, u_int8_t mode, u_int32_t period, u_int32_t filesize, u_int64_t unique_id) {

    char filename[100];
    strcpy(filename, "output/download-times");
    switch (mode)
    {
    case 1:
        strcat(filename, "-DL");
        break;
    
    case 2:
        strcat(filename, "-UL");
        break;
    
    default:
        break;
    }
    char temp[12];
    itoa(period, temp, 10);
    strcat(filename, "-");
    strcat(filename, temp);
    strcat(filename, "ms");
    
    itoa(filesize/1024, temp, 10);
    strcat(filename, "-");
    strcat(filename, temp);
    strcat(filename, "kB");
    
    itoa( (u_int32_t) unique_id, temp, 10);
    strcat(filename, "-");
    strcat(filename, temp);

    strcat(filename, ".txt");

    FILE *fp;
    fp = fopen(filename, "a+");
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

/**
	 * C++ version 0.4 char* style "itoa":
	 * Written by Lukás Chmela
	 * Released under GPLv3.

	 */
char* itoa(int value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}