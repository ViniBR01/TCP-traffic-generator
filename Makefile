#reference: https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
SERVER= start-server
CLIENT= start-client

TARGETS= $(SERVER) $(CLIENT)

SRC= main.c client.c server.c
OBJ= $(SRC:.c=.o)
HDR= $(SRC:.c=.h)

CC= gcc
CFLAGS= -I.

all: $(TARGETS)

$(SERVER): $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

.c.o: $(HDR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(CLIENT): $(SERVER)
	ln -f $< $@

.PHONY: clean

clean:
	rm -f $(OBJ) *~ 