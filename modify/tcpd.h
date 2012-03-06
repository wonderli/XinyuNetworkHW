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
#include <errno.h>
#endif
#include "port.h"
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
//typedef struct packet {
//        struct sockaddr_in msg_header;
//        PACKET_DATA msg;
//        unsigned long checksum;
//} PACKET;
typedef struct TCPD_MSG {
        //struct sockaddr_in tcpd_header;
	//struct packet packet;
        struct sockaddr_in tcpd_header;
        PACKET_DATA packet;
        unsigned short checksum;
} TCPD_MSG;
typedef struct troll_msg {
	struct sockaddr_in troll_header;
	struct TCPD_MSG tcpd_msg;
} TROLL_MSG;

int SEND(int socket, const void *buffer, size_t len, int flags);
int RECV(int socket, void *buffer, size_t length, int flags);
int RECV_CONTROL(int socket, void *buffer, size_t len, int flags);
float RTO(float M, int packet_ctrl);
float RTT(struct timeval *start, struct timeval *end);

