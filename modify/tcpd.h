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
typedef struct packet_data {
        int seq_num;
        int ack;
        int ack_seq;
        int length;
        int fin;
        int fin_ack;
        int stop;
        char data[MAXBUF];
} PACKET_DATA;
typedef struct packet {
        struct sockaddr_in msg_header;
        PACKET_DATA msg;
        unsigned long checksum;
} PACKET;
typedef struct TCPD_MSG {
        struct sockaddr_in tcpd_header;
        char tcpd_contents[sizeof(struct packet)];

} TCPD_MSG;
typedef struct troll_msg {
	struct sockaddr_in troll_header;
	char msg_contents[sizeof(struct TCPD_MSG)];
} TROLL_MSG;

int SEND(int socket, const void *buffer, size_t len, int flags);
int RECEIVE(int socket, void *buffer, size_t length, int flags);
