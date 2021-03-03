#reference: https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
SERVER= start-server
CLIENT= start-client

TARGETS= $(SERVER) $(CLIENT)

SRC= main.c client.c server.c
OBJ= $(SRC:.c=.o)
DEPS= $(SRC:.c=.h)

VPATH= .:cli_parser:client_core:server_core:output:utils
CC= gcc
CFLAGS= -I. -I./cli_parser -I./client_core -I./server_core -I./output -I./utils

all: $(TARGETS)

$(SERVER): $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

.c.o: $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(CLIENT): $(SERVER)
	ln -f $< $@

.PHONY: clean

clean:
	rm -f $(OBJ) *~ 