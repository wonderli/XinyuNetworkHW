#ifndef _NET_HEADER_
#define _NET_HEADER_
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#endif
#define TCPD_PORT 1110
#define TROLL_PORT 2110
#define MAXBUF 1000

int SEND(int socket, const void *buffer, size_t len, int flags);
int RECEIVE(int socket, void *buffer, size_t length, int flags);
