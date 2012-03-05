/* server.c using TCP */

/* Server for accepting an Internet stream connection on port 1040 */
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
/*#define port "1050"*/   /* socket file name */
#define MAXBUF 1000
#endif
#include "tcpd.h"
/* server program called with no argument */
int main(int argc, char *argv[])
{
	int sock;                     /* initial socket descriptor */
	int msgsock;                  /* accepted socket descriptor,
				       * each client connection has a
				       *                                  * unique socket descriptor*/
	int rval=1;                   /* returned value from a read */  
	struct sockaddr_in sin_addr; /* structure for socket name setup */
	char* buf;
	buf = (char*)malloc(MAXBUF);
	TCPD_MSG recv_msg;
	if(argc < 2) {
		printf("Usage: ftps <local-port>\n");
		exit(1);
	}
	printf("TCP server waiting for remote connection from clients ...\n");

	/*initialize socket connection in unix domain*/
	/*if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){ */
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("error openting datagram socket");
		exit(1);
	}

	/* construct name of socket to send to */
	sin_addr.sin_family = AF_INET;
	sin_addr.sin_addr.s_addr = INADDR_ANY;
	/*sin_addr.sin_port = htons(atoi(port));*/
	sin_addr.sin_port = htons(atoi(argv[1]));
	int opt=1;

        if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int)) == -1) {
                perror("Setsockopt Failed");
                exit(1);
        }

	/* bind socket name to socket */
//	if(bind(sock, (struct sockaddr *)&sin_addr, sizeof(struct sockaddr_in)) < 0) {
//		perror("error binding stream socket");
//		exit(1);
//	}

        /* put all zeros in buffer (clear) */
	bzero(buf,MAXBUF);
	/* read from msgsock and place in buf */
	int nread = 0; /* the number read from socket*/
        bzero(&recv_msg, sizeof(TCPD_MSG));
	if((nread = RECV(sock, &recv_msg, sizeof(TCPD_MSG),0)) < 0) {
		perror("error reading on stream socket");
		exit(1);
	} 
//	nread = RECV(sock, &recv_msg, sizeof(TCPD_MSG),0);
	printf("\nServer receives: SEQ %d\n", recv_msg.packet.seq_num);
        char *filename;
        char *filepath;
        uint32_t file_size = 0;
        uint32_t file_size_local = 0;
        filename = (char*)malloc(20);
        filepath = (char*)malloc(MAXBUF);
        //bcopy(buf, &file_size, sizeof(int));
        bcopy(recv_msg.packet.data, &file_size, sizeof(int));
        file_size_local = ntohl(file_size);
        //bcopy(buf+4, filename, 20);
        bcopy(recv_msg.packet.data+4, filename, 20);
        strcpy(filepath, "./recv/");
        strcat(filepath, filename);
        printf("The file length is %d\n", file_size_local);
        printf("The file name is %s\n", filepath);
	/*Create the receive file folder and file*/
	int fd = 0;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	if(access("recv",F_OK) == -1)
	{
		mkdir("recv",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
	if ((fd = open(filepath, O_WRONLY|O_CREAT, mode))< 0)
	{
		perror("File open error");
		return 1;
	}

	//if(write(fd,buf+24, nread-24) < 0)
//	if((nread = RECV(sock, &recv_msg, sizeof(TCPD_MSG),0)) < 0) {
//		perror("error reading on stream socket");
//		exit(1);
//	} 
//
//	if(write(fd,&recv_msg.packet.data, sizeof(recv_msg.packet.data) < 0))
//	{
//		perror("error on write file");
//		exit(1);
//	}
//        struct sockaddr_in test_addr;
//        test_addr.sin_family = AF_INET;
//        test_addr.sin_port = htons(TCPD_PORT);
//        test_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//        int test_addr_length = sizeof(struct sockaddr);

	for(;;)
	{
		bzero(recv_msg.packet.data, MAXBUF);

		RECV(sock, &recv_msg, sizeof(TCPD_MSG), 0);
		if(recv_msg.packet.fin == 1)
		{
			printf("\nFile: %s, length: %d has been transmitted!\n", filename, file_size_local);
			close(fd);
			close(sock);
			exit(0);

		}
			write(fd, recv_msg.packet.data, sizeof(recv_msg.packet.data));

	}
	
	/* close all connections and remove socket file */
	return 0;
}

