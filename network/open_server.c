/* open_server.c - TCP traffic generator */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "open_server.h"
#include "config_messages.h"

/**************************************************/
/* a few simple linked list functions             */
/**************************************************/

/* remove the data structure associated with a connected socket
   used when tearing down the connection */
void dump(struct node *head, int socket) {
  struct node *current, *temp;

  current = head;

  while (current->next) {
    if (current->next->socket == socket) {
      /* remove */
      temp = current->next;
      current->next = temp->next;
      free(temp); /* don't forget to free memory */
      return;
    } else {
      current = current->next;
    }
  }
}

/* create the data structure associated with a connected socket */
void add(struct node *head, int socket, struct sockaddr_in addr) {
  struct node *new_node;

  new_node = (struct node *)malloc(sizeof(struct node));
  new_node->socket = socket;
  new_node->client_addr = addr;
  new_node->pending_data = 0;
  new_node->next = head->next;
  head->next = new_node;
}

server_t * setup_server(unsigned int port) {
    server_t *server_options = (server_t *) malloc(sizeof(server_t));

    /* socket and option variables */
    int sock;
    int optval = 1;

    /* server socket address variables */
    struct sockaddr_in sin;
    unsigned short server_port = (unsigned short) port;

    /* socket address variables for a connected client */
    socklen_t addr_len = sizeof(struct sockaddr_in);

    /* maximum number of pending connection requests */
    int BACKLOG = 5;

    /* a buffer to read data */
    char *buf;
    int BUF_LEN = 1500;

    buf = (char *)malloc(BUF_LEN); /* XXX fix-me: receive buflen as param */
    if (!buf) {
        perror("failed to allocate buffer.");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    /* linked list for keeping track of connected sockets */
    struct node *head = (struct node *) malloc(sizeof(struct node));
    /* initialize dummy head node of linked list */
    head->socket = -1;
    head->next = NULL;

    /* create a server socket to listen for TCP connection requests */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror ("error opening TCP socket.");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    /* set option so we can reuse the port number quickly after a restart */
    if (setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)) <0) {
        perror("error setting TCP socket option.");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    /* set option so we turn off the delayed TCP ack in this socket */
    if (setsockopt (sock, IPPROTO_TCP, TCP_QUICKACK, &optval, sizeof (optval)) <0) {
        perror("error setting TCP socket option.");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    /* fill in the address of the server socket */
    memset (&sin, 0, sizeof (sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons (server_port);
  
    /* bind server socket to the address */
    if (bind(sock, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
        perror("binding socket to address");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    /* put the server socket in listen mode */
    if (listen (sock, BACKLOG) < 0) {
        perror ("listen on socket failed.");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    server_options->socket = sock;
    server_options->recv_buffer = buf;
    server_options->open_sockets = head;

    return server_options;
}

void check_connections(void *server_options, unsigned int task_id) {
    
    server_t *server_options_in = (server_t *) server_options;
    int sock = server_options_in->socket;
    int new_sock, max, count;
    int optval = 1;

    /* linked list for keeping track of connected sockets */
    struct node *head = server_options_in->open_sockets;
    struct node *current, *next;
    
    /* a buffer to read data */
    char *buf = server_options_in->recv_buffer;
    int BUF_LEN = 1000;

    /* now we check for incoming connections,
        check for incoming data to receive,
        check for ready socket to send more data */
    
    /* variables for select */
    fd_set read_set, write_set;
    struct timeval time_out;
    int select_retval;

    /* set up the file descriptor bit map that select should be watching */
    FD_ZERO (&read_set); /* clear everything */
    FD_ZERO (&write_set); /* clear everything */

    FD_SET (sock, &read_set); /* put the listening socket in */
    max = sock; /* initialize max */

    /* put connected sockets into the read and write sets to monitor them */
    for (current = head->next; current; current = current->next) {
        FD_SET(current->socket, &read_set);

        if (current->pending_data) {
            /* there is data pending to be sent, monitor the socket
                in the write set so we know when it is ready to take more
                data */
            FD_SET(current->socket, &write_set);
        }

        if (current->socket > max) {
        /* update max if necessary */
        max = current->socket;
        }
    }

    time_out.tv_usec = 100000; /* 1-tenth of a second timeout */
    time_out.tv_sec = 0;

    /* invoke select, make sure to pass max+1 !!! */
    select_retval = select(max+1, &read_set, &write_set, NULL, &time_out);
    if (select_retval < 0) {
        perror ("select failed.");
        exit(EXIT_FAILURE);
        /* NOTREACHED */
    }

    if (select_retval == 0) {
        /* no descriptor ready, timeout happened */
        return;
    }
    
    if (select_retval > 0) {
        /* at least one file descriptor is ready */
        /* check the server socket */
        if (FD_ISSET(sock, &read_set)) {
            /* there is an incoming connection, try to accept it */
            struct sockaddr_in addr;
            socklen_t addr_len = sizeof(struct sockaddr_in);
            new_sock = accept (sock, (struct sockaddr *) &addr, &addr_len);
        
            if (new_sock < 0) {
                perror ("error accepting connection.");
                exit(EXIT_FAILURE);
                /* NOTREACHED */
            }

            /* make the socket non-blocking so send and recv will
                return immediately if the socket is not ready.
                this is important to ensure the server does not get
                stuck when trying to send data to a socket that
                has too much data to send already.
                */
            if (fcntl (new_sock, F_SETFL, O_NONBLOCK) < 0) {
                perror ("error making socket non-blocking.");
                exit(EXIT_FAILURE);
                /* NOTREACHED */
            }

            /* set option so we turn off the delayed TCP ack in this socket */
            if (setsockopt (sock, 
                            IPPROTO_TCP, 
                            TCP_QUICKACK, 
                            &optval, 
                            sizeof (optval)) <0) {
                perror("error setting TCP socket option.");
                exit(EXIT_FAILURE);
                /* NOTREACHED */
            }

            /* the connection is made, everything is ready */
            /* let's see who's connecting to us */
            printf("Accepted connection. Client IP address is: %s\n",
                inet_ntoa(addr.sin_addr));

            /* remember this client connection in our linked list */
            add(head, new_sock, addr);
        }

        /* check other connected sockets, see if there is
            anything to read or some socket is ready to send
            more pending data */
        for (current = head->next; current; current = next) {
            next = current->next;

            /* see if we can now do some previously unsuccessful writes */
            if (FD_ISSET(current->socket, &write_set)) {
                /* the socket is now ready to take more data */
                /* the socket data structure should have information
                    describing what data is supposed to be sent next.
                    but here for simplicity, let's say we are just
                    sending whatever is in the buffer buf
                */
                count = send(current->socket, buf, BUF_LEN, MSG_DONTWAIT);
                if (count < 0) {
                    if (errno == EAGAIN) {
                    /* we are trying to dump too much data down the socket,
                        it cannot take more for the time being 
                        will have to go back to select and wait til select
                        tells us the socket is ready for writing
                    */
                    } else {
                        /* something else is wrong */
                        perror("error sending message to client.");
                        exit(EXIT_FAILURE);
                        /* NOTREACHED */
                    }
                }
            }
            /* note that it is important to check count for exactly
                how many bytes were actually sent even when there are
                no error. send() may send only a portion of the buffer
                to be sent.
            */

            if (FD_ISSET(current->socket, &read_set)) {
                /* we have data from a client */
        
                count = recv(current->socket, buf, BUF_LEN, 0);

                if (count <= 0) {
                    /* something is wrong */
                    if (count == 0) {
                        printf("Client closed connection. Client IP address is: %s\n", inet_ntoa(current->client_addr.sin_addr));
                    } else {
                        perror("error receiving from a client");
                    }

                    /* connection is closed, clean up */
                    close(current->socket);
                    dump(head, current->socket);
                } else {
                    /* we got count bytes of data from the client */
                    //XXX Here must access the type field and do a switch case statement to handle types
                    uint8_t message_type = buf[0];
                    switch (message_type)
                    {
                    case MSGTYPE_SETUP:
                        printf("Received a config message.\n");
                        printf("Message length = %d\n", ntohl(*(uint32_t *)(buf+1)));
                        break;
                    
                    case MSGTYPE_DIGEST:
                        printf("Received a DIGEST message.\n");
                        printf("Message length = %d\n", ntohl(*(uint32_t *)(buf+1)));
                        break;

                    default:
                        break;
                    }
                    /* XXX finish-me: should have a status for file and check if transmissions ended */
                    
                    /* a complete message is received, print it out */
                    /*printf("Received a file chunck with size \"%d\". Client IP address is: %s\n",
                        count, inet_ntoa(current->client_addr.sin_addr));*/
                }
            }
        }
    }
}