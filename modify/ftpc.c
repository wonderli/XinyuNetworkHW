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
int file_send (int sck,int control_sck, char *filename, struct sockaddr_in sin_addr, struct sockaddr_in control_addr);
/* client program called with host name where server is run */
int main(int argc, char *argv[])
{
	int sock;                     /* initial socket descriptor */
	int  rval;                    /* returned value from a read */
	struct sockaddr_in sin_addr; /* structure for socket name 
				      * setup */
        struct sockaddr_in tcpd_addr;
        struct sockaddr_in control_addr;
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
        
        int sock_control;
        if((sock_control = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                perror("error openting datagram socket");
                exit(1);
        }
        control_addr.sin_family = AF_INET;
        control_addr.sin_port = htons(CONTROL_PORT);
        control_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if(bind(sock_control, (struct sockaddr *)&control_addr, sizeof(struct sockaddr_in)) < 0) {
		perror("error binding stream socket");
		exit(1);
	}

	file_send(sock,sock_control, filename,sin_addr, control_addr);  

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
int file_send (int sck, int control_sck, char *filename, struct sockaddr_in sin_addr, struct sockaddr_in control_addr)
{
        printf("\nBEGIN TO SEND FILE\n");
        int nread;
        int send_file;
        fd_set read_fds;

        char *read_file_buf;
        read_file_buf = (char*) malloc(MAXBUF);

//        TCPD_MSG tcpd_msg;
        TCPD_MSG send_msg;
        TCPD_MSG recv_msg;

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

//        bcopy(&file_size, read_file_buf, sizeof(int));
//        bcopy(filename, read_file_buf+4, 20);
        bzero(&send_msg, sizeof(TCPD_MSG));

        send_msg.tcpd_header = sin_addr;
        send_msg.packet.seq_num = 0;

        bcopy(&file_size, read_file_buf, sizeof(int));
        bcopy(filename, read_file_buf+4, 20);
        bcopy(send_msg.packet.data, read_file_buf, 24);
        SEND(sck,&send_msg,sizeof(TCPD_MSG),0);
        bzero(read_file_buf, MAXBUF);

        int FILE_EOF = FALSE;
        
        FD_ZERO(&read_fds);
        FD_SET(control_sck, &read_fds);
        while(1)
        {
                if(select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0)
                {
                        perror("SELECT ERROR");
                        exit(1);
                }
                if(FD_ISSET(control_sck, &read_fds))
                {
                        RECV_CONTROL(control_sck, &recv_msg, sizeof(TCPD_MSG), 0);
                        if(recv_msg.packet.stop == 1)
                        {
                                printf("\nreceive stop signal, stop sending msg\n");
                                usleep(10000);
                        }
                        else
                        {
                                if ((nread = read (send_file, read_file_buf, MAXBUF)) < MAXBUF)
                                {
                                        send_msg.packet.length = nread;
                                        send_msg.packet.seq_num = 1;
                                        bcopy(read_file_buf,send_msg.packet.data, nread+24);
                                        //SEND (sck, (char *)&send_msg, nread+24+sizeof(struct sockaddr_in), 0);
                                        SEND (sck, (char *)&send_msg, sizeof(send_msg), 0);
                                        usleep(10000);

                                        bzero(send_msg.packet.data, MAXBUF);
                                        send_msg.packet.fin = 1;
                                        send_msg.packet.length = 0;
                                        send_msg.packet.seq_num++;
                                        SEND (sck, (char *)&send_msg, sizeof(send_msg), 0);

                                }
                                else if (nread == (MAXBUF-24))
                                {
                                        send_msg.packet.seq_num = 1;
                                        send_msg.packet.length = nread+24;
                                        bcopy(read_file_buf,send_msg.packet.data,nread+24);
                                        //SEND (sck, (char *)&send_msg, MAXBUF+sizeof(struct sockaddr_in), 0);
                                        SEND (sck, (char *)&send_msg, sizeof(send_msg), 0);
                                        bzero(read_file_buf, MAXBUF);

//                                        usleep(10000);
                                        for(;;)
                                        {
                                                if ((nread = read (send_file, read_file_buf, MAXBUF)) < MAXBUF)
                                                {
                                                        send_msg.packet.seq_num ++;
                                                        send_msg.packet.length = nread;
                                                        bcopy(read_file_buf,send_msg.packet.data,nread);
                                                        SEND (sck, (char *)&send_msg, sizeof(send_msg), 0);
                                                        /*printf("The nread is %d", nread);*/
                                                        bzero(read_file_buf, MAXBUF);
//                                                        usleep(10000);
                                                        FILE_EOF = TRUE;
                                                        break;
                                                }
                                                else
                                                {
                                                        send_msg.packet.seq_num++;
                                                        send_msg.packet.length = nread;
                                                        bcopy(read_file_buf,send_msg.packet.data,MAXBUF);
                                                        SEND (sck, (char *)&send_msg, sizeof(send_msg), 0);
                                                        bzero(read_file_buf, MAXBUF);
//                                                        usleep(10000);
                                                }
                                        } 
                                        if(FILE_EOF == TRUE)
                                        {
                                                bzero(send_msg.packet.data, MAXBUF);
                                                send_msg.packet.fin = 1;
                                                send_msg.packet.length = 0;
                                                send_msg.packet.seq_num++;
                                                SEND (sck, (char *)&send_msg, sizeof(send_msg), 0);
                                                close(send_file);
                                                close(sck);
                                        }
                                }//END else if

                        }//end else

                }//END FD_ISSET
                FD_ZERO(&read_fds);
                FD_SET(control_sck, &read_fds);

        }//END while
        return 0;
}


























//        if ((nread = read (send_file, read_file_buf+24, MAXBUF-24)) < (MAXBUF-24))
//        {
////                bcopy(read_file_buf,tcpd_msg.packet.data, nread+24);
//                SEND (sck, (char *)&tcpd_msg, nread+24+sizeof(struct sockaddr_in), 0);
//                usleep(10000);
//        }
//        else if (nread == (MAXBUF-24))
//        {
//                bcopy(read_file_buf,tcpd_msg.tcpd_contents,nread+24);
//                SEND (sck, (char *)&tcpd_msg, MAXBUF+sizeof(struct sockaddr_in), 0);
//                bzero(read_file_buf, MAXBUF);
//
//                usleep(10000);
//                for(;;)
//                {
//                        if ((nread = read (send_file, read_file_buf, MAXBUF)) < MAXBUF)
//                        {
//                                bcopy(read_file_buf,tcpd_msg.tcpd_contents,nread);
//                                SEND (sck, (char *)&tcpd_msg, nread+sizeof(struct sockaddr_in), 0);
//                                /*printf("The nread is %d", nread);*/
//                                bzero(read_file_buf, MAXBUF);
//                                usleep(10000);
//                                break;
//                        }
//                        else
//                        {
//                                bcopy(read_file_buf,tcpd_msg.tcpd_contents,MAXBUF);
//                                SEND (sck, (char *)&tcpd_msg, MAXBUF+sizeof(struct sockaddr_in), 0);
//                                bzero(read_file_buf, MAXBUF);
//                                usleep(10000);
//                        }
//                } 
//        }
//close (send_file);

