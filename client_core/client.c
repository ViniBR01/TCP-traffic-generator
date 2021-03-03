/* client.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "client.h"

#define LENGTH 1024 //buffer
#define PORT 8081

/* XXX do not use globals */
int fsize;
int portn;

int client(options_t *options) {
    printf("Inside of the client function.\n");

    if (!options) {
        errno = EINVAL;
        return EXIT_FAILURE;
    }

    /* XXX do needful stuff */
    fsize = options->file_size;
    portn = options->port;

    int sock_fd = 0;
    int valread;
    char buffer[1024] = {0};
    char *hello = "Hello from client";
    struct sockaddr_in serv_addr;

    //Open socket and store file descriptor
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
        printf("\n Socket creation error \n");
        return -1;
        }

    //Set address of server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portn);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
        {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
        }

    //Connect to server
    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
        printf("\nConnection Failed \n");
        return -1;
        }

    //Test connection by sending a short message:
    send(sock_fd , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");

    while(1) {
        //Create file named 'receive.txt'. If already existant, first delete file.
        char* f_name = "receive.txt";

        int status = remove(f_name);
        if (status == 0)
        printf("File deleted successfully.\n");
        else
        {
        printf("Unable ro delete file.\n");
        //exit(0);
        }

        //Receive data from server and save to file 
        int X = fsize - 1;
        char revbuf[LENGTH];
        FILE *fp = fopen(f_name, "a");
        bzero(revbuf, LENGTH);
        int f_block_sz = 0;
        int success = 0;

        //First file:
        while (success == 0)
        {
        //printf("Inside while of success == 0.\n");

        int total_rcv = 0;
        while (total_rcv < X)
            {
            //printf("Inside while of recv() cmd.\n");
            //printf("X - total_rcv = %d\n", X - total_rcv);
            f_block_sz = recv(sock_fd, revbuf, LENGTH, 0);
            total_rcv += f_block_sz;

            int write_sz = fwrite(revbuf, sizeof(char), f_block_sz, fp);
            if (write_sz < f_block_sz)
            {
            printf("Error\n");
            exit(0);
            }
            bzero(revbuf, LENGTH);
            }
        success = 1;
        fclose(fp);
        }
        }
    
    return EXIT_SUCCESS;
}