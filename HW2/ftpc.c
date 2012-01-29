/* client.c using TCP */

/* Client for connecting to Internet stream server waiting on port 1040 */
#ifndef _NET_HEADER_
#define _NET_HEADER_
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
/*#define port "1050" */   /* socket file name */
#define MAXBUF 1000
#endif
#include "tcpd.h"
/*
* int file_send(int sck, char *file);
*/
int file_send (int sck, char *filename, struct sockaddr_in sin_addr);
/* client program called with host name where server is run */
int main(int argc, char *argv[])
{
	int sock;                     /* initial socket descriptor */
	int  rval;                    /* returned value from a read */
	struct sockaddr_in sin_addr; /* structure for socket name 
				      * setup */
        struct sockaddr_in tcpd_addr;
	struct hostent *hp;
	char filename[MAXBUF];
	if(argc < 4) {
		printf("Usage: ftpc <remote-IP> <remote-port> <loca-file-transfer>\n");
		exit(1);
	}
	strcpy(filename, argv[3]);
	/* initialize socket connection in unix domain */
	/*if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){*/
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("error openting datagram socket");
		exit(1);
	}

	hp = gethostbyname(argv[1]);
	if(hp == 0) {
		fprintf(stderr, "%s: unknown host\n", argv[1]);
		exit(2);
	}

	/* construct name of socket to send to */
	bcopy((void *)hp->h_addr, (void *)&sin_addr.sin_addr, hp->h_length);
	sin_addr.sin_family = AF_INET;
	sin_addr.sin_port = htons(atoi(argv[2])); /* fixed by adding htons() */

        /*tcpd address setup*/
        tcpd_addr.sin_family = AF_INET;
        tcpd_addr.sin_port = htons(TCPD_PORT);
        tcpd_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	file_send(sock,filename,sin_addr);  

        return 0;

}
int get_file_size(char *filename)
{
        FILE *fp;
	int start = 0; /*Read start point*/
	int file_size = 0;
	fp = fopen(filename, "rb");
	if(fp == NULL)
	{
		perror("File not found!!!");
	}
	else
	{
		start = ftell(fp);
		fseek(fp, 0L, SEEK_END);
		file_size = ftell(fp);
		fseek(fp, start,SEEK_SET);
	}
	fclose(fp);
	return file_size;
}
/*
* int file_send (int sck, char *filename)
*/
int file_send (int sck, char *filename, struct sockaddr_in sin_addr)
{
        int nread;
        int send_file;

        char *read_file_buf;
        read_file_buf = (char*) malloc(MAXBUF);

        TCPD_MSG tcpd_msg;
        tcpd_msg.tcpd_header = sin_addr;

        if ((send_file = open (filename, O_RDONLY) )< 0)
        {
                perror ("File open error");
                return 1;
        }

        bzero(read_file_buf, MAXBUF);

        int file_size_local = get_file_size(filename);
        uint32_t file_size = htonl(file_size_local);

        int sockaddr_in_size = sizeof(struct sockaddr_in);

        printf("filesize is %d\n", file_size_local);
        printf("filename is %s\n", filename);
        //        bcopy(&sin_addr, read_file_buf, sockaddr_in_size);
        //        bcopy(&file_size, read_file_buf+sockaddr_in_size, sizeof(int));
        //        bcopy(filename, read_file_buf+sockaddr_in_size+4, 20);
        bcopy(&file_size, read_file_buf, sizeof(int));
        bcopy(filename, read_file_buf+4, 20);
        bzero(&tcpd_msg, sizeof(TCPD_MSG));



        if ((nread = read (send_file, read_file_buf+24, MAXBUF-24)) < (MAXBUF-24))
        {
                //send (sck, read_file_buf, nread+24+sockaddr_in_size, 0);
                bcopy(read_file_buf,tcpd_msg.tcpd_contents, nread+24);
                //SEND (sck, read_file_buf, nread+24+sockaddr_in_size, 0);
                SEND (sck, (char *)&tcpd_msg, nread+24+16, 0);
        }
        else if (nread == (MAXBUF-24))
        {
                //send (sck, read_file_buf, nread+24+sockaddr_in_size, 0);
                //SEND (sck, read_file_buf, nread+24+sockaddr_in_size, 0);
                bcopy(read_file_buf,tcpd_msg.tcpd_contents,nread+24);
                SEND (sck, (char *)&tcpd_msg, MAXBUF+16, 0);
                bzero(read_file_buf, MAXBUF);

                for(;;)
                {
                        if ((nread = read (send_file, read_file_buf, MAXBUF)) < MAXBUF)
                        {
                                bcopy(read_file_buf,tcpd_msg.tcpd_contents,nread);
                                //send (sck, read_file_buf, nread, 0);
                                //SEND (sck, read_file_buf, nread, 0);
                                SEND (sck, (char *)&tcpd_msg, nread+16, 0);
                                /*printf("The nread is %d", nread);*/
                                bzero(read_file_buf, MAXBUF);
                                break;
                        }
                        else
                        {
                                bcopy(read_file_buf,tcpd_msg.tcpd_contents,MAXBUF);
                                //send (sck, read_file_buf, MAXBUF, 0);
                                //SEND (sck, read_file_buf, MAXBUF, 0);
                                SEND (sck, (char *)&tcpd_msg, MAXBUF+16, 0);
                                bzero(read_file_buf, MAXBUF);
                        }
                } 
        }
close (send_file);
return 0;
}
