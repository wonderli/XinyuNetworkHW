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
	if(argc < 2) {
		printf("Usage: ftps <local-port>\n");
		exit(1);
	}
	printf("TCP server waiting for remote connection from clients ...\n");

	/*initialize socket connection in unix domain*/
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("error openting datagram socket");
		exit(1);
	}

	/* construct name of socket to send to */
	sin_addr.sin_family = AF_INET;
	sin_addr.sin_addr.s_addr = INADDR_ANY;
	/*sin_addr.sin_port = htons(atoi(port));*/
	sin_addr.sin_port = htons(atoi(argv[1]));

	/* bind socket name to socket */
	if(bind(sock, (struct sockaddr *)&sin_addr, sizeof(struct sockaddr_in)) < 0) {
		perror("error binding stream socket");
		exit(1);
	}

	/* listen for socket connection and set max opened socket connetions to 5 */
	listen(sock, 5);

	/* accept a (1) connection in socket msgsocket */ 
	if((msgsock = accept(sock, (struct sockaddr *)NULL, (int *)NULL)) == -1) { 
		perror("error connecting stream socket");
		exit(1);
	} 

	/* put all zeros in buffer (clear) */
	bzero(buf,MAXBUF);
	/* read from msgsock and place in buf */
	int nread = 0; /* the number read from socket*/
	if((nread = read(msgsock, buf, MAXBUF)) < 0) {
		perror("error reading on stream socket");
		exit(1);
	} 
	/*printf("Server receives: %s\n", buf);*/
	char *filename;
	char *filepath;
	uint32_t file_size = 0;
	uint32_t file_size_local = 0;
	filename = (char*)malloc(20);
	filepath = (char*)malloc(MAXBUF);
        struct sockaddr_in server_addr_recv;
        int server_addr_recv_size = sizeof(struct sockaddr_in);
        bcopy(buf, &server_addr_recv, server_addr_recv_size);
	bcopy(buf+server_addr_recv_size, &file_size, sizeof(int));
	file_size_local = ntohl(file_size);
	bcopy(buf+server_addr_recv_size+4, filename, 20);
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

	if(write(fd,buf+server_addr_recv_size+24, nread-24-server_addr_recv_size) < 0)
	{
		perror("error on write file");
		exit(1);
	}
	if(nread == MAXBUF)
	{
		for(;;)
		{
			//if((nread = read(msgsock, buf, MAXBUF)) < MAXBUF)
			if((nread = recv(msgsock, buf, MAXBUF, 0)) < MAXBUF)
			{
				write(fd, buf, nread);
				/*printf("The nread is %d", nread);*/
				break;

			}
			else 
				write(fd, buf, MAXBUF);

		}
	}
	close(fd);
	/* write message back to client */
	char *buf2;
	buf2 = (char *)malloc(MAXBUF);
	strcpy(buf2, "Transfer finished\n");
	if(write(msgsock, buf2, MAXBUF) < 0) {
		perror("error writing on stream socket");
		exit(1);
	}
	/*printf("\n%s\n", buf2);*/
	printf("\nFile: %s, length: %d has been transmitted!\n", filename, file_size_local);
	
	/* close all connections and remove socket file */
	close(msgsock);
	close(sock);
	return 0;
}

