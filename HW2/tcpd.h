#ifndef _NET_H
#define _NET_H
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
#define TCPD_PORT 3860
#define TROLL_PORT_M1 3870
#define TROLL_PORT_M2 3880
#define MAXBUF 1000
typedef struct TCPD_MSG {
        struct sockaddr_in tcpd_header;
        char tcpd_contents[MAXBUF];
} TCPD_MSG;

int SEND(int socket, const void *buffer, size_t len, int flags);
int RECEIVE(int socket, void *buffer, size_t length, int flags);
