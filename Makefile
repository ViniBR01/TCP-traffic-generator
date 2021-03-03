# Makefile for TCP Traffic Generation
#references: 
#https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
#https://www.gnu.org/software/make/manual/make.html#Directory-Search

SERVER= start-server
CLIENT= start-client

TARGETS= $(SERVER) $(CLIENT)

VPATH= .:cli_parser:client_core:server_core:output:utils
ODIR= obj
CFLAGS= -I. -I./cli_parser -I./client_core -I./server_core -I./output -I./utils

SRC= main.c client.c server.c
_OBJ= $(SRC:.c=.o)
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
DEPS= $(SRC:.c=.h)

CC= gcc

LIBS=

all: $(TARGETS)

$(SERVER): $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(CLIENT): $(SERVER)
	ln -f $< $@

.PHONY: clean

clean:
	rm -f $(OBJ) *~ 