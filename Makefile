CC=gcc
CFLAGS=-I.

main: main.o
	$(CC) -o main main.o -I.