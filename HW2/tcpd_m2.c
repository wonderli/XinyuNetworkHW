/*client part of TCPD*/
/*connection port is 3110*/
#include "tcpd.h"
#include "troll.h"
int main() /* server program called with no argument */
{
        int sock_ftpc, ftpc_addr_len, buflen;
        char ftpc_buf1[MAXBUF], ftpc_buf[MAXBUF];
        int sock_troll, sock_to_troll_len;
        struct sockaddr_in ftpc_addr;
        struct sockaddr_in troll_addr;
        /*create from ftpc socket*/
        sock_ftpc = socket(AF_INET, SOCK_DGRAM, 0);
        if(sock_ftpc < 0) {
                perror("opening datagram socket for recv from ftpc");
                exit(1);
        }
        /*create troll socket*/
        sock_troll = socket(AF_INET, SOCK_DGRAM, 0);
        if(sock_troll < 0) {
                perror("opening datagram socket for send to troll");
                exit(1);
        }

        /* create ftpc_addr with parameters and bind ftpc_addr to socket */
        ftpc_addr.sin_family = AF_INET;
        ftpc_addr.sin_port = htons(TCPD_PORT);
        ftpc_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if(bind(sock_ftpc, (struct sockaddr *)&ftpc_addr, sizeof(ftpc_addr)) < 0) {
                perror("Recv(receive from ftpc) socket Bind failed");
                exit(2);
        }
        /* create troll_addr with parameters and bind troll_addr to socket */
        /*
        troll_addr.sin_family = AF_INET;
        troll_addr.sin_port = htons(TROLL_PORT);
        troll_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if(bind(sock_troll, (struct sockaddr *)&troll_addr, sizeof(troll_addr)) < 0) {
                perror("Send(send to troll) socket Bind failed");
                exit(2);
        }
        */
        ftpc_addr_len=sizeof(struct sockaddr_in);
        sock_to_troll_len=sizeof(struct sockaddr_in);
        buflen = 0;
        bzero(ftpc_buf1, MAXBUF);
        bzero(ftpc_buf, MAXBUF);
        if((buflen = recvfrom(sock_ftpc, ftpc_buf1, MAXBUF, 0, (struct sockaddr *)&ftpc_addr, &ftpc_addr_len)) < 0){
                        perror("error receiving from ftpc"); 
                        exit(4);
        }
        /* Get ftps IP address, struct sockaddr_in*/
        struct sockaddr_in ftps_addr;
        int ftps_addr_size = sizeof(struct sockaddr_in);
        bcopy(ftpc_buf1, &ftps_addr, ftps_addr_size);
        bcopy(ftpc_buf1+ftps_addr_size, &ftpc_buf1, MAXBUF);
        char *ftpc_recv_filename;
        ftpc_recv_filename = (char*) malloc(20);
        strcpy(ftpc_recv_filename, "./recv/received");
        int fd = 0;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
        if((fd = open(ftpc_recv_filename, O_WRONLY|O_CREAT, mode))< 0)
        {
                perror("File open error");
                return 1;
        }
        if(write(fd,ftpc_buf+24, buflen-24-ftps_addr_size) < 0)
	{
		perror("error on write file");
		exit(1);
        }
        if(buflen == MAXBUF)
        {
                for(;;)
                {
                        //if((nread = read(msgsock, buf, MAXBUF)) < MAXBUF)
                        if((buflen = recvfrom(sock_ftpc, ftpc_buf1, MAXBUF, 0, (struct sockaddr *)&ftpc_addr, &ftpc_addr_len)) < MAXBUF){
                                /*        if((nread = recv(msgsock, buf, MAXBUF, 0)) < MAXBUF) */
                                write(fd, ftpc_buf, buflen);
                                /*printf("The nread is %d", nread);*/
                                break;

                        }
                        else 
                        write(fd, ftpc_buf, MAXBUF);

                }
        }





        






        /*
        while(1) {
                if((buflen = recvfrom(sock_ftpc, ftpc_buf, MAXBUF, 0, (struct sockaddr *)&ftpc_addr, &ftpc_addr_len)) < 0){
                        perror("error receiving from ftpc"); 
                        exit(4);
                }
                if((sendto(sock_troll, ftpc_buf, buflen, 0, (struct sockaddr *)&troll_addr, sock_to_troll_len)) < 0){
                        perror("sending datagram to troll");
                        exit(5);
                }
                bzero(ftpc_buf, buflen);
                printf("Server receives: %s\n", ftpc_buf);
        }
        */
        close(sock_ftpc);
        close(sock_troll);
        exit(0);
}
