/* server.c using TCP */

/* Server for accepting an Internet stream connection on port 1040 */
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
#define port "1050"   /* socket file name */
#define MAXBUF 1024

/* server program called with no argument */
int main(void)
{
	int sock;                     /* initial socket descriptor */
	int msgsock;                  /* accepted socket descriptor,
				       * each client connection has a
				       *                                  * unique socket descriptor*/
	int rval=1;                   /* returned value from a read */  
	struct sockaddr_in sin_addr; /* structure for socket name setup */
	char* buf;
	buf = (char*)malloc(MAXBUF);
	printf("TCP server waiting for remote connection from clients ...\n");

	/*initialize socket connection in unix domain*/
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("error openting datagram socket");
		exit(1);
	}

	/* construct name of socket to send to */
	sin_addr.sin_family = AF_INET;
	sin_addr.sin_addr.s_addr = INADDR_ANY;
	sin_addr.sin_port = htons(atoi(port));

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
	if(read(msgsock, buf, MAXBUF) < 0) {
		perror("error reading on stream socket");
		exit(1);
	} 
	printf("Server receives: %s\n", buf);
	char *filename;
	char *filepath;
	int file_size = 0;
	filename = (char*)malloc(20);
	filepath = (char*)malloc(MAXBUF);
	bcopy(buf, &file_size, sizeof(int));
	bcopy(buf+4, filename, 20);
	strcpy(filepath, "./recv/");
	strcat(filepath, filename);
	printf("The file length is %d\n", file_size);
	printf("The file name is %s\n", filepath);
	
	int fd = 0;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	if(access("recv",F_OK) == -1)
	{
		mkdir("recv",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
	if (fd = open(filepath, O_CREAT, mode) < 0)
	{
		perror("File open error");
		return 1;
	}

	write(fd,buf+24, MAXBUF);
	close(fd);
	/* write message back to client */
	char *buf2;
	buf2 = (char *)malloc(MAXBUF);
	strcpy(buf2, "Transfer finished\n");
	if(write(msgsock, buf2, 1024) < 0) {
		perror("error writing on stream socket");
		exit(1);
	}
	printf("\n%s\n", buf2);
	
	/* close all connections and remove socket file */
	close(msgsock);
	close(sock);
	return 0;
}

