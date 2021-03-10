/* Script to test the server implementation */
//to compile:
//gcc -o test test_server.c server.c ./../utils/scheduler.c ./../network/send_file.c -I. -I./../utils -I./../network

#include <stdio.h>
#include <stdlib.h>

#include "server.h"
#include "scheduler.h"

int main() {
    printf("Starting test: server.c.\n");
    options_t options = { 0, 8080, "", 1000, 10 };
    server(&options);
    return 0;
}