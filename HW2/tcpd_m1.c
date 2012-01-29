/*client part of TCPD*/
/*connection port is 3110*/
#include "tcpd.h"
#include "troll.h"
int main() /* server program called with no argument */
{
        int sock_ftps, ftps_addr_len, buflen;
        char ftps_buf1[MAXBUF], ftps_buf[MAXBUF];
        int sock_troll, sock_from_troll_len;
        struct sockaddr_in ftps_addr;
        struct sockaddr_in troll_addr;
        NetMessage troll_msg;
        TCPD_MSG tcpd_msg;
        /*create from ftpc socket*/
        sock_ftps = socket(AF_INET, SOCK_DGRAM, 0);
        if(sock_ftps < 0) {
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
        ftps_addr.sin_family = AF_INET;
        ftps_addr.sin_port = htons(TCPD_PORT);
        ftps_addr.sin_addr.s_addr = inet_addr("127.0.0.2");
       /* create troll_addr with parameters and bind troll_addr to socket */
        
        troll_addr.sin_family = AF_INET;
        troll_addr.sin_port = htons(TROLL_PORT_M1);
        troll_addr.sin_addr.s_addr = INADDR_ANY;
        if(bind(sock_troll, (struct sockaddr *)&troll_addr, sizeof(troll_addr)) < 0) {
                perror("Recv(receive from troll) socket Bind failed");
                exit(2);
        }

        ftps_addr_len=sizeof(struct sockaddr_in);
        sock_from_troll_len=sizeof(struct sockaddr_in);
        buflen = 0;
        //bzero(ftps_buf1, MAXBUF);
        //bzero(ftps_buf, MAXBUF);
        if((buflen = recvfrom(sock_troll, (char *)&troll_msg, sizeof(NetMessage), 0, (struct sockaddr *)&troll_addr, &sock_from_troll_len)) < 0){
                        perror("error receiving from troll"); 
                        exit(4);
        }
        //struct sockaddr_in ftps_addr;
        //int ftps_addr_size = sizeof(struct sockaddr_in);
        //bcopy(ftpc_buf1, &ftps_addr, ftps_addr_size);
        //bcopy(ftpc_buf1+ftps_addr_size, &ftpc_buf, MAXBUF);
        //bzero(troll_msg.msg_contents,MAXBUF);

        //troll_msg.msg_header = ftps_addr;
        bzero(ftps_buf, MAXBUF);
        bcopy(ftps_buf, &troll_msg.msg_contents, buflen-16);
        /*

        if((sendto(sock_ftps, &ftps_buf, buflen-16, 0, (struct sockaddr *)&ftps_addr, ftps_addr_len)) < 0){
                perror("sending datagram to ftps");
                exit(5);
        }
        */
   
        /* The following part is for write file recv data from ftpc*/
        
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
        //if(write(fd,ftps_buf+24, buflen-24-ftps_addr_size) < 0)
        if(write(fd,ftps_buf+24, buflen-24) < 0)
	{
		perror("error on write file");
		exit(1);
        }

//        if(buflen == MAXBUF)
//        {
//                for(;;)
//                {
//                        //if((nread = read(msgsock, buf, MAXBUF)) < MAXBUF)
//                        if((buflen = recvfrom(sock_ftpc, ftpc_buf1, MAXBUF, 0, (struct sockaddr *)&ftpc_addr, &ftpc_addr_len)) < MAXBUF){
//                                bcopy(ftpc_buf1, &troll_msg.msg_contents, buflen);
//                                if((sendto(sock_troll, (char*)&troll_msg, sizeof(troll_msg), 0, (struct sockaddr *)&troll_addr, sock_to_troll_len)) < 0){
//                                        perror("sending datagram to troll");
//                                        exit(5);
//                                }
//                                printf("line number 101, Sent to troll\n");
//
//                                /*        if((nread = recv(msgsock, buf, MAXBUF, 0)) < MAXBUF) */
//                                /*write(fd, ftpc_buf, buflen);*/
//                                /*printf("The nread is %d", nread);*/
//                                break;
//
//                        }
//                        else
//                        {
//                                /*
//                                * write(fd, ftpc_buf, MAXBUF);
//                                */
//                                bcopy(ftpc_buf1, &troll_msg.msg_contents, MAXBUF);
//                                if((sendto(sock_troll, (char*)&troll_msg, sizeof(troll_msg), 0, (struct sockaddr *)&troll_addr, sock_to_troll_len)) < 0){
//                                        perror("sending datagram to troll");
//                                        exit(5);
//                                }
//                                printf("line number 119, Sent to troll\n");
//                        }
//
//
//                }
//        }


        close(sock_troll);
        close(sock_ftps);
        exit(0);
}
