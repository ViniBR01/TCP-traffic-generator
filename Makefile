#reference: https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
CC=gcc
CFLAGS=-I.

main: main.o
	$(CC) -o main main.o -I.