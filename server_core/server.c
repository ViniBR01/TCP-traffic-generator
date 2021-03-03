/* server.c - TCP traffic generator */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "server.h"
#include "scheduler.h"

#define LENGTH 1024 //buffer
#define PORT 8081
#define PORT 8081
#define LENGTH 1024

/* Do not use globals XXX */
int period;
int fsize;
int portn;
int new_socket;

void taskA(void *p);
void taskB(void *p);
void taskC(void *p);

int server(options_t *options) {

    printf("Inside of the server function.\n");

    if (!options) {
        errno = EINVAL;
        return EXIT_FAILURE;
    }

    /* XXX do server stuff */
    //Read arguments from argv[]
    period = options->period;
    fsize = options->file_size;
    portn = options->port;
    
    /*Initialize tasks*/////////////////////////////////////////////////////////////////////////////////
    int j = 0;

    TaskList[j].ftpr = taskA;
    TaskList[j].arg_ptr = NULL;
    TaskList[j].state = STATE_READY;
    TaskList[j].delay = -1;
    j++;

    TaskList[j].ftpr = taskB;
    int task_B_Arg = 56;		//arbitrary number 56
    int *ipB = &task_B_Arg;
    TaskList[j].arg_ptr = (void *) ipB;
    TaskList[j].state = STATE_INACTIVE;
    TaskList[j].delay = -1;
    j++;

    TaskList[j].ftpr = taskC;
    int task_C_Arg = 56;		//arbitrary number 56
    int *ipC = &task_C_Arg;
    TaskList[j].arg_ptr = (void *) ipC;
    TaskList[j].state = STATE_INACTIVE;
    TaskList[j].delay = -1;
    j++;

    TaskList[j].ftpr = NULL;

    /*Initialize socket and file*////////////////////////////////////////////////////////////////////////
    int server_fd;
    //int new_socket;
    int valread;
    int opt = 1;
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    //create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
        //handle error
        perror("socket failed");
        exit(EXIT_FAILURE);
        }

    //Set address of server
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( portn );
    
    

    //Attach socket to server address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
        {
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
        } 

    //Start listening for incoming connections
    if (listen(server_fd, 3) < 0) 
        { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
        } 

    //Accept an incoming connection and register new_socket descriptor
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
        {
        perror("accept"); 
        exit(EXIT_FAILURE); 
        }

    //Read an incoming message from socket
    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    
    //Now start scheduler
    while(1) {
        scheduler();
    }

    return EXIT_SUCCESS;
}

void taskA(void *p){
  printf("This is the beggining of task A.\n");
  //Call taskC with parameter = 1:
  TaskList[2].state = STATE_WAITING; //taskC
  //Select random phase
  int phase = rand() % period;
  TaskList[2].delay = phase;
  gettimeofday(&TaskList[2].delay_ref, NULL);
  printf("taskC will start in %d microseconds.\n", phase);
  delay(period); //3.0 secs
  printf("This is the end of task A.\n");
}

void taskB(void *p){
  printf("This is the beggining of task B.\n");
  printf("Task B parameter is equals to: %d\n", *( (int*) TaskList[task_index].arg_ptr));
  delay(1000000); //1.0 secs
  //*( (int*) TaskList[task_index].arg_ptr) = *( (int*) TaskList[task_index].arg_ptr) + 1;
  printf("This is the end of task B.\n");
  halt_me();
}

void taskC(void *p){
  printf("This is the beggining of task C.\n");
  printf("Task C parameter is equals to: %d\n", *( (int*) TaskList[task_index].arg_ptr));
  //delay(1000000); //1.0 secs
  //*( (int*) TaskList[task_index].arg_ptr) = *( (int*) TaskList[task_index].arg_ptr) + 1;

  //Create file and send it over the network
  //Create file of a given size X and move pointer to the beggining of the file
  char* f_name = "test.txt";
  int status = remove(f_name);
  if (status == 0)
    printf("File deleted successfully.\n");
  else
    {
      printf("Unable to delete file.\n");
    }
  int X = fsize - 1; //Size = 10kB

  FILE *fp = fopen(f_name, "w");

  for (int i = 0; i<=X; i++)
    {
      fprintf(fp, "b");
    }
  //close file
  fclose(fp);

  //////////////////////////////////////////////
  //Send file that was just created to client:
  int success = 0;
  while(success == 0)
  {
    char sdbuf[LENGTH]; // Send buffer
    printf("[server] send %s to the client...\n", f_name);
    fp = fopen(f_name, "r");
    if(fp == NULL)
      {
	printf("ERROR: File %s not found.\n", f_name);
	exit(1);
      }
    bzero(sdbuf, LENGTH);
    int f_block_sz;

    f_block_sz = fread(sdbuf, sizeof(char), LENGTH, fp);
    //printf("Going to while loop. Read one block from the file. Return value = %d.\n", f_block_sz);
    while(f_block_sz>0)
      {
	//printf("TEST: entering while loop for sending file. f_block_sx = %d\n", f_block_sz);
	if(send(new_socket, sdbuf, f_block_sz, 0) < 0)
	  {
	    printf("TEST: Entering if (send) statement.\n");
	    printf("ERROR: Failed to send file %s.\n", f_name);
	    break;
	  }
	//printf("TEST: After if (send) statement.\n");
	bzero(sdbuf, LENGTH);

	f_block_sz = fread(sdbuf, sizeof(char), LENGTH, fp);
	//printf("TEST: End of while loop. Read one block from the file. Return value = %d.\n", f_block_sz);
      }
    printf("ok!\n");
    success = 1;
    while(waitpid(-1, NULL, WNOHANG) > 0);
  }

	
  printf("This is the end of task C.\n\n");
  halt_me();
}