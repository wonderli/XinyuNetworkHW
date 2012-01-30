/*client part of TCPD*/
/*connection port is 3110*/
#include "tcpd.h"
#include "troll.h"
int main() /* server program called with no argument */
{
        int sock_ftps, ftps_addr_len;
        int buflen = 0;
        //char troll_buf1[sizeof(TCPD_MSG)], troll_buf[sizeof(TCPD_MSG)];
        //char troll_buf[sizeof(TCPD_MSG)];
        char troll_buf[sizeof(NetMessage)];
        int sock_troll, troll_addr_len;
        struct sockaddr_in ftps_addr;
        struct sockaddr_in troll_addr;
        NetMessage troll_msg;
        TCPD_MSG tcpd_msg;
        /*create from ftpc socket*/
        if((sock_ftps = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for recv from ftpc");
                exit(1);
        }
        /*create troll socket*/
        if((sock_troll = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for send to troll");
                exit(1);
        }

        /* create troll_addr with parameters and bind troll_addr to socket */
        troll_addr.sin_family = AF_INET;
        troll_addr.sin_port = htons(TROLL_PORT_M1);
//        troll_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        troll_addr.sin_addr.s_addr = INADDR_ANY;        
        if(bind(sock_troll, (struct sockaddr *)&troll_addr, sizeof(troll_addr)) < 0) {
                perror("Recv(receive from troll) socket Bind failed");
                exit(2);
        }

        ftps_addr_len=sizeof(struct sockaddr_in);
        troll_addr_len=sizeof(struct sockaddr_in);

        //bzero(troll_buf1, MAXBUF);
        bzero(troll_buf, MAXBUF);
        bzero(tcpd_msg.tcpd_contents, MAXBUF);
        bzero(troll_msg.msg_contents,MAXBUF);

        if((buflen = recvfrom(sock_troll, troll_buf, sizeof(troll_buf), 0, (struct sockaddr *)&troll_addr, &troll_addr_len)) < 0){
                perror("error receiving from ftpc"); 
                exit(4);
        }
        bcopy(troll_buf, &troll_msg,buflen);
        bcopy(&troll_msg.msg_contents, &tcpd_msg, sizeof(troll_msg.msg_contents));
        //        bcopy(troll_buf, &tcpd_msg, buflen);
        /* Get ftps IP address, struct sockaddr_in*/
        int ftps_addr_size = sizeof(struct sockaddr_in);
        ftps_addr = tcpd_msg.tcpd_header;
        /* create ftps_addr with parameters and bind ftps_addr to socket */
        ftps_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        /* The following part is for write file recv data from ftpc*/
        //        char *ftpc_recv_filename;
        //        ftpc_recv_filename = (char*) malloc(20);
        //        strcpy(ftpc_recv_filename, "./recv/troll");
        //        int fd = 0;
        //	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
        //        if((fd = open(ftpc_recv_filename, O_WRONLY|O_CREAT, mode))< 0)
        //        {
        //                perror("File open error");
        //                return 1;
        //        }
        /*
        if(write(fd,tcpd_msg.tcpd_contents+24, buflen-24-16) < 0)
        {
        perror("error on write file");
        exit(1);
        }
        */
        //if((sendto(sock_ftps, ftps_buf, sizeof(ftps_buf), 0, (struct sockaddr *)&ftps_addr, ftps_addr_len)) < 0){
        if((sendto(sock_ftps, &tcpd_msg.tcpd_contents, buflen-16-16, 0, (struct sockaddr *)&ftps_addr, ftps_addr_len)) < 0){
                perror("sending datagram to ftps");
                exit(5);
        }
        bzero(&tcpd_msg,sizeof(TCPD_MSG));


        if(buflen == sizeof(NetMessage))
//        if(buflen == sizeof(TCPD_MSG))
        {

                for(;;)
                {
                        if((buflen = recvfrom(sock_troll, troll_buf, sizeof(troll_buf), 0, (struct sockaddr *)&troll_addr, &troll_addr_len)) < (MAXBUF+16+16)){
                                bcopy(troll_buf,&troll_msg, buflen);
                                bcopy(&troll_msg.msg_contents, &tcpd_msg, buflen-16);
                                //bcopy(troll_buf,&tcpd_msg, buflen);
                                //bcopy(&troll_msg.msg_contents, &tcpd_msg, buflen-16);

                                //if((sendto(sock_ftps, &tcpd_msg.tcpd_contents, buflen-16-16, 0, (struct sockaddr *)&ftps_addr, ftps_addr_len)) < 0){
                                if((sendto(sock_ftps, &tcpd_msg.tcpd_contents, buflen-16-16, 0, (struct sockaddr *)&ftps_addr, ftps_addr_len)) < 0){
                                        perror("sending datagram to ftps");
                                        exit(5);
                                }
                                bzero(&tcpd_msg,sizeof(TCPD_MSG));

                                //write(fd,tcpd_msg.tcpd_contents, buflen-16);
                                break;

                        }
                        else
                        {
                                bcopy(troll_buf,&troll_msg, MAXBUF+16+16);
                                bcopy(&troll_msg.msg_contents, &tcpd_msg, MAXBUF+16);
                                //                                bcopy(troll_buf,&tcpd_msg, buflen);

                                //write(fd,tcpd_msg.tcpd_contents, MAXBUF);
                                if((sendto(sock_ftps, &tcpd_msg.tcpd_contents, MAXBUF, 0, (struct sockaddr *)&ftps_addr, ftps_addr_len)) < 0){
                                        perror("sending datagram to ftps");
                                        exit(5);
                                }
                                bzero(&tcpd_msg,sizeof(TCPD_MSG));
                        }


                }
        }


        close(sock_troll);
       close(sock_ftps);
        exit(0);
        }
