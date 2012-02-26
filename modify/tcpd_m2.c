/*client part of TCPD*/
/*connection port is 3110*/
#include "tcpd.h"
#include "troll.h"
int main() /* server program called with no argument */
{
        int sock_ftpc, ftpc_addr_len;
        int buflen = 0;
        //char ftpc_buf1[sizeof(TCPD_MSG)], ftpc_buf[sizeof(TCPD_MSG)];
        char ftpc_buf[sizeof(TCPD_MSG)];
        int sock_troll, troll_addr_len;
        struct sockaddr_in ftpc_addr;
        struct sockaddr_in troll_addr;
        NetMessage troll_msg;
        TCPD_MSG tcpd_msg;
        /*create from ftpc socket*/
        if((sock_ftpc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for recv from ftpc");
                exit(1);
        }
        /*create troll socket*/
        if((sock_troll = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for send to troll");
                exit(1);
        }

        /* create ftpc_addr with parameters and bind ftpc_addr to socket */
        ftpc_addr.sin_family = AF_INET;
        ftpc_addr.sin_port = htons(TCPD_PORT);
        ftpc_addr.sin_addr.s_addr = INADDR_ANY;
        if(bind(sock_ftpc, (struct sockaddr *)&ftpc_addr, sizeof(ftpc_addr)) < 0) {
                perror("Recv(receive from ftpc) socket Bind failed");
                exit(2);
        }
        /* create troll_addr with parameters */
        troll_addr.sin_family = AF_INET;
        //troll_addr.sin_port = htons(TROLL_PORT_M2);
        troll_addr.sin_port = htons(TROLL_PORT_M2);
        troll_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        ftpc_addr_len=sizeof(struct sockaddr_in);
        troll_addr_len=sizeof(struct sockaddr_in);

        //bzero(ftpc_buf1, MAXBUF);
        bzero(ftpc_buf, MAXBUF);
        bzero(tcpd_msg.tcpd_contents, MAXBUF);
        bzero(troll_msg.msg_contents,MAXBUF);

        if((buflen = recvfrom(sock_ftpc, ftpc_buf, sizeof(ftpc_buf), 0, (struct sockaddr *)&ftpc_addr, &ftpc_addr_len)) < 0){
                perror("error receiving from ftpc"); 
                exit(4);
        }
        bcopy(ftpc_buf,&tcpd_msg, sizeof(ftpc_buf));
        /* Get ftps IP address, struct sockaddr_in*/
        struct sockaddr_in ftps_addr;
        int ftps_addr_size = sizeof(struct sockaddr_in);

        ftps_addr = tcpd_msg.tcpd_header;

       /* Change tcpd_m1 port to satisfy troll*/
        ftps_addr.sin_port = htons(TROLL_PORT_M1);
        troll_msg.msg_header = ftps_addr;
        //bcopy((char*)&tcpd_msg, &troll_msg.msg_contents, sizeof(troll_msg));
        bcopy((char*)&tcpd_msg, &troll_msg.msg_contents, sizeof(tcpd_msg));

        /* The following part is for write file recv data from ftpc*/

//       char *ftpc_recv_filename;
//       ftpc_recv_filename = (char*) malloc(20);
//       strcpy(ftpc_recv_filename, "./recv/troll");
//       int fd = 0;
//       mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
//       if((fd = open(ftpc_recv_filename, O_WRONLY|O_CREAT, mode))< 0)
//       {
//               perror("File open error");
//               return 1;
//       }
//       //if(write(fd,tcpd_msg.tcpd_contents+24, buflen-24-ftps_addr_size) < 0)
//       if(write(fd,tcpd_msg.tcpd_contents+24, buflen-24-16) < 0)
//       {
//               perror("error on write file");
//               exit(1);
//       }
       if((sendto(sock_troll, (char*)&troll_msg, buflen+16+16, 0, (struct sockaddr *)&troll_addr, troll_addr_len)) < 0){
               perror("sending datagram to troll");
               exit(5);
       }


        if(buflen == sizeof(TCPD_MSG))
        {
                for(;;)
                {
                        //if((nread = read(msgsock, buf, MAXBUF)) < MAXBUF)
                        //if((buflen = recvfrom(sock_ftpc, ftpc_buf1, MAXBUF, 0, (struct sockaddr *)&ftpc_addr, &ftpc_addr_len)) < MAXBUF){
                        if((buflen = recvfrom(sock_ftpc, ftpc_buf, sizeof(ftpc_buf), 0, (struct sockaddr *)&ftpc_addr, &ftpc_addr_len)) < (MAXBUF+16)){
                                //bcopy(ftpc_buf1, &troll_msg.msg_contents, buflen);
                                bcopy(ftpc_buf,&tcpd_msg, buflen);
                                bcopy((char*)&tcpd_msg, &troll_msg.msg_contents, sizeof(tcpd_msg));
                               if((sendto(sock_troll, (char*)&troll_msg, buflen+16, 0, (struct sockaddr *)&troll_addr, troll_addr_len)) < 0){
                                       perror("sending datagram to troll");
                                       exit(5);
                               }
//                                write(fd,tcpd_msg.tcpd_contents, buflen-16);
                                break;

                        }
                        else
                        {
                                /*
                                * write(fd, ftpc_buf, MAXBUF);
                                */
                                //bcopy(ftpc_buf1, &troll_msg.msg_contents, MAXBUF);
                                bcopy(ftpc_buf,&tcpd_msg, MAXBUF+16);
                                bcopy((char*)&tcpd_msg, &troll_msg.msg_contents, MAXBUF+16);
//                                write(fd,tcpd_msg.tcpd_contents, MAXBUF);
                                if((sendto(sock_troll, (char*)&troll_msg, MAXBUF+16+16, 0, (struct sockaddr *)&troll_addr, troll_addr_len)) < 0){
                                        perror("sending datagram to troll");
                                        exit(5);
                                }
                        }


                }
        }


        close(sock_ftpc);
        close(sock_troll);
        exit(0);
        }
