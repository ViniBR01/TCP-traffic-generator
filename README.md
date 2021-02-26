# TCP traffic generator
UNIX based traffic generator for experiments with TCP

This project implements a CLI program that generates TCP traffic for networking experiments.
The main point of this program is to generate a random and statistically independent sequence of file downloads and uploads.
The file size and the frequency of files generation are parameters to the program that can be passed at the command line or in a configuration file.

This program can run as a passive server that waits for connections in a TCP socket, or a client that connects to a remote server and passes the configuration information.