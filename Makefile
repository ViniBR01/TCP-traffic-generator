# Makefile for TCP Traffic Generation
#references: 
#https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
#https://www.gnu.org/software/make/manual/make.html#Directory-Search

SERVER= start-server
CLIENT= start-client

TARGETS= $(SERVER) $(CLIENT)

VPATH= .:core:input:output:network:traffic_models:utils
ODIR= obj
CFLAGS= -I. \
		-I./input \
		-I./core \
		-I./output \
		-I./network \
		-I./traffic_models \
		-I./utils \

SRC= 	main.c client.c server.c \
		send_file.c open_server.c config_messages.c \
		traffic_factory.c m_fixed_periodic.c \
		scheduler.c log_file.c\
		
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