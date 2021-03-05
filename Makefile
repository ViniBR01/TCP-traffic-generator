# Makefile for TCP Traffic Generation
#references: 
#https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
#https://www.gnu.org/software/make/manual/make.html#Directory-Search

SERVER= start-server
CLIENT= start-client

TARGETS= $(SERVER) $(CLIENT)

VPATH= .:core:input:output:traffic_model:utils
ODIR= obj
CFLAGS= -I. -I./input -I./core -I./output -I./traffic_model -I./utils

SRC= main.c client.c server.c scheduler.c traffic_factory.c
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