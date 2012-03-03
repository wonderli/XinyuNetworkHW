/*client part of TCPD*/
/*connection port is 3110*/
#include "tcpd.h"
#include "troll.h"
#include "deltalist.h"
struct TCPD_MSG buffer[64];
int window[20];
int locate_in_buffer(int seq_num)
{
        int i = 0;
        for(i = 0; i < 64; i++)
        {
                if(buffer[i].packet.seq_num == seq_num) break;
        }
        return i;
}

int main() /* server program called with no argument */
{
        int sock_ftpc, ftpc_addr_len;
        int buflen = 0;
        int sock_troll, troll_addr_len;
        int sock_control, control_addr_len;
        int sock_ack, ack_addr_len;
        int sock_timer_send;
        int sock_timer_recv;

        struct sockaddr_in ftpc_addr;
        struct sockaddr_in control_addr;
        struct sockaddr_in troll_m1_addr;
        struct sockaddr_in troll_m2_addr;
        struct sockaddr_in timer_send_addr;
        struct sockaddr_in timer_recv_addr;

        fd_set read_fds;

        TCPD_MSG tcpd_msg;
        //TROLL_MSG troll_msg;

        TIME_MSG timer_send;
        TIME_MSG timer_recv;
        
        struct timeval time_start, time_end, diff;
        float start, end;
        int resend_pkt;
        int head = 0;
        int tail = 0;
        int ptr = 0;
        int index = 0;



        /*create from ftpc socket*/
        if((sock_ftpc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for recv from ftpc");
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

        /*create control to socket*/
        if((sock_control = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for recv from ftpc");
                exit(1);
        }
        control_addr.sin_family = AF_INET;
        control_addr.sin_port = htons(CONTROL_PORT_M2);
        control_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if((sock_ack = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for recv from ftpc");
                exit(1);
        }
        ack_addr.sin_family = AF_INET;
        ack_addr.sin_port = htons(TROLL_PORT_M1);
        ack_addr.sin_addr.s_addr = INADDR_ANY;
        if(bind(sock_ack, (struct sockaddr *)&ack_addr, sizeof(ack_addr)) < 0) {
                perror("ACK socket Bind failed");
                exit(2);
        }


        if((sock_timer_send = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for recv from timer send");
                exit(1);
        }
        timer_send_addr.sin_family = AF_INET;
        timer_send_addr.sin_port = htons(TIMER_SEND_PORT);
        timer_send_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if((sock_timer_recv = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for recv from timer recv");
                exit(1);
        }       
        timer_recv_addr.sin_family = AF_INET;
        timer_recv_addr.sin_port = htons(TIMER_RECV_PORT);
        timer_recv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        if(bind(sock_timer_recv, (struct sockaddr *)&timer_recv_addr, sizeof(timer_recv_addr)) < 0) {
                perror("timer recv socket Bind failed");
                exit(2);
        }

        /*create troll socket*/
        if((sock_troll = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for send to troll");
                exit(1);
        }

        /* create troll_addr with parameters */
        troll_addr.sin_family = AF_INET;
        troll_addr.sin_port = htons(TROLL_PORT_M2);
        troll_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        ftpc_addr_len=sizeof(struct sockaddr_in);
        troll_addr_len=sizeof(struct sockaddr_in);

//        if((buflen = recvfrom(sock_ftpc, (void *)&tcpd_msg, sizeof(TCPD_MSG), 0, (struct sockaddr *)&ftpc_addr, &ftpc_addr_len)) < 0){
//                perror("error receiving from ftpc"); 
//                exit(4);
//        }
        /* Get ftps IP address, struct sockaddr_in*/
        struct sockaddr_in ftps_addr;
        int ftps_addr_size = sizeof(struct sockaddr_in);
        struct hostent *hp;
        hp = gethostbyname(argv[1]);
        if(hp == 0)
        {
                perror("Unknown host");
                exit(1);
        }
        ftps_addr.sin_family = htons(AF_INET);
        bcopy(hp->h_addr, (char*) ftps_addr, hp->h_length);
        ftps_addr.sin_port = htons(TCPD_PORT_M1);


        if((sock_ack = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for send to troll");
                exit(1);
        }

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


        FD_ZERO(&read_fds);
        FD_SET(sock_ftpc, &read_fds);
        FD_SET(sock_timer_recv, &read_fds);
        FD_SET(sock_ack, &read_fds);
        while(1)
        {
                if(select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0)
                {
                        perror("SELECT ERROR");
                        exit(0);
                }
                if(FD_ISSET(sock_ftpc, &read_fds))
                {
                        buflen = recvfrom(sock_ftpc, (void *)&buffer[head], sizeof(TCPD_MSG), 0, (struct sockaddr *)&ftpc_addr, &ftpc_addr_len);
                        window[ptr] = buffer[head].packet.seq_num;
                        buffer[head].tcpd_header = ftps_addr;
                        buffer[head].checksum = crc((void *)&buffer[head].packet, sizeof(struct packet_data));
                        index = locate_in_buffer(window[ptr]);
                        sendto(sock_troll, (void *)buffer[index], sizeof(TCPD_MSG), 0, (struct sockaddr *)&troll_addr, troll_addr_len);

                        

                }
        

        }





























       if((sendto(sock_troll, (char*)&troll_msg, buflen+sizeof(struct sockaddr_in)+sizeof(struct sockaddr_in), 0, (struct sockaddr *)&troll_addr, troll_addr_len)) < 0){
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
                               //if((sendto(sock_troll, (char*)&troll_msg, buflen+16, 0, (struct sockaddr *)&troll_addr, troll_addr_len)) < 0){
                               if((sendto(sock_troll, (char*)&troll_msg, buflen+sizeof(struct sockaddr_in), 0, (struct sockaddr *)&troll_addr, troll_addr_len)) < 0){
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
                                
				//bcopy(ftpc_buf,&tcpd_msg, MAXBUF+16);
                                
				bcopy(ftpc_buf,&tcpd_msg, MAXBUF+sizeof(struct sockaddr_in));
                                bcopy((char*)&tcpd_msg, &troll_msg.msg_contents, MAXBUF+sizeof(struct sockaddr_in));
//                                write(fd,tcpd_msg.tcpd_contents, MAXBUF);
                                
				//if((sendto(sock_troll, (char*)&troll_msg, MAXBUF+16+16, 0, (struct sockaddr *)&troll_addr, troll_addr_len)) < 0){
                                if((sendto(sock_troll, (char*)&troll_msg, MAXBUF+sizeof(struct sockaddr_in)+sizeof(struct sockaddr_in), 0, (struct sockaddr *)&troll_addr, troll_addr_len)) < 0){
                                        perror("sending datagram to troll");
                                        exit(5);
                                }
                        }


                }
        }


        close(sock_ftpc);
        close(sock_troll);
        exit(0);



























//        int sock_ftpc, ftpc_addr_len;
//        int buflen = 0;
//        //char ftpc_buf1[sizeof(TCPD_MSG)], ftpc_buf[sizeof(TCPD_MSG)];
//        char ftpc_buf[sizeof(TCPD_MSG)];
//        int sock_troll, troll_addr_len;
//        struct sockaddr_in ftpc_addr;
//        struct sockaddr_in troll_addr;
//        NetMessage troll_msg;
//        TCPD_MSG tcpd_msg;
//        /*create from ftpc socket*/
//        if((sock_ftpc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
//                perror("opening datagram socket for recv from ftpc");
//                exit(1);
//        }
//        /*create troll socket*/
//        if((sock_troll = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
//                perror("opening datagram socket for send to troll");
//                exit(1);
//        }
//
//        /* create ftpc_addr with parameters and bind ftpc_addr to socket */
//        ftpc_addr.sin_family = AF_INET;
//        ftpc_addr.sin_port = htons(TCPD_PORT);
//        ftpc_addr.sin_addr.s_addr = INADDR_ANY;
//        if(bind(sock_ftpc, (struct sockaddr *)&ftpc_addr, sizeof(ftpc_addr)) < 0) {
//                perror("Recv(receive from ftpc) socket Bind failed");
//                exit(2);
//        }
//        /* create troll_addr with parameters */
//        troll_addr.sin_family = AF_INET;
//        //troll_addr.sin_port = htons(TROLL_PORT_M2);
//        troll_addr.sin_port = htons(TROLL_PORT_M2);
//        troll_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//
//        ftpc_addr_len=sizeof(struct sockaddr_in);
//        troll_addr_len=sizeof(struct sockaddr_in);
//
//        //bzero(ftpc_buf1, MAXBUF);
//        bzero(ftpc_buf, MAXBUF);
//        bzero(tcpd_msg.tcpd_contents, MAXBUF);
//        bzero(troll_msg.msg_contents,MAXBUF);
//
//        if((buflen = recvfrom(sock_ftpc, ftpc_buf, sizeof(ftpc_buf), 0, (struct sockaddr *)&ftpc_addr, &ftpc_addr_len)) < 0){
//                perror("error receiving from ftpc"); 
//                exit(4);
//        }
//        bcopy(ftpc_buf,&tcpd_msg, sizeof(ftpc_buf));
//        /* Get ftps IP address, struct sockaddr_in*/
//        struct sockaddr_in ftps_addr;
//        int ftps_addr_size = sizeof(struct sockaddr_in);
//
//        ftps_addr = tcpd_msg.tcpd_header;
//
//       /* Change tcpd_m1 port to satisfy troll*/
//        ftps_addr.sin_port = htons(TROLL_PORT_M1);
//        troll_msg.msg_header = ftps_addr;
//        //bcopy((char*)&tcpd_msg, &troll_msg.msg_contents, sizeof(troll_msg));
//        bcopy((char*)&tcpd_msg, &troll_msg.msg_contents, sizeof(tcpd_msg));
//
//        /* The following part is for write file recv data from ftpc*/
//
////       char *ftpc_recv_filename;
////       ftpc_recv_filename = (char*) malloc(20);
////       strcpy(ftpc_recv_filename, "./recv/troll");
////       int fd = 0;
////       mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
////       if((fd = open(ftpc_recv_filename, O_WRONLY|O_CREAT, mode))< 0)
////       {
////               perror("File open error");
////               return 1;
////       }
////       //if(write(fd,tcpd_msg.tcpd_contents+24, buflen-24-ftps_addr_size) < 0)
////       if(write(fd,tcpd_msg.tcpd_contents+24, buflen-24-16) < 0)
////       {
////               perror("error on write file");
////               exit(1);
////       }
//
//
//       //if((sendto(sock_troll, (char*)&troll_msg, buflen+16+16, 0, (struct sockaddr *)&troll_addr, troll_addr_len)) < 0){
//       if((sendto(sock_troll, (char*)&troll_msg, buflen+sizeof(struct sockaddr_in)+sizeof(struct sockaddr_in), 0, (struct sockaddr *)&troll_addr, troll_addr_len)) < 0){
//               perror("sending datagram to troll");
//               exit(5);
//       }
//
//
//        if(buflen == sizeof(TCPD_MSG))
//        {
//                for(;;)
//                {
//                        //if((nread = read(msgsock, buf, MAXBUF)) < MAXBUF)
//                        //if((buflen = recvfrom(sock_ftpc, ftpc_buf1, MAXBUF, 0, (struct sockaddr *)&ftpc_addr, &ftpc_addr_len)) < MAXBUF){
//                        if((buflen = recvfrom(sock_ftpc, ftpc_buf, sizeof(ftpc_buf), 0, (struct sockaddr *)&ftpc_addr, &ftpc_addr_len)) < (MAXBUF+16)){
//                                //bcopy(ftpc_buf1, &troll_msg.msg_contents, buflen);
//                                bcopy(ftpc_buf,&tcpd_msg, buflen);
//                                bcopy((char*)&tcpd_msg, &troll_msg.msg_contents, sizeof(tcpd_msg));
//                               //if((sendto(sock_troll, (char*)&troll_msg, buflen+16, 0, (struct sockaddr *)&troll_addr, troll_addr_len)) < 0){
//                               if((sendto(sock_troll, (char*)&troll_msg, buflen+sizeof(struct sockaddr_in), 0, (struct sockaddr *)&troll_addr, troll_addr_len)) < 0){
//                                       perror("sending datagram to troll");
//                                       exit(5);
//                               }
////                                write(fd,tcpd_msg.tcpd_contents, buflen-16);
//                                break;
//
//                        }
//                        else
//                        {
//                                /*
//                                * write(fd, ftpc_buf, MAXBUF);
//                                */
//                                //bcopy(ftpc_buf1, &troll_msg.msg_contents, MAXBUF);
//                                
//				//bcopy(ftpc_buf,&tcpd_msg, MAXBUF+16);
//                                
//				bcopy(ftpc_buf,&tcpd_msg, MAXBUF+sizeof(struct sockaddr_in));
//                                bcopy((char*)&tcpd_msg, &troll_msg.msg_contents, MAXBUF+sizeof(struct sockaddr_in));
////                                write(fd,tcpd_msg.tcpd_contents, MAXBUF);
//                                
//				//if((sendto(sock_troll, (char*)&troll_msg, MAXBUF+16+16, 0, (struct sockaddr *)&troll_addr, troll_addr_len)) < 0){
//                                if((sendto(sock_troll, (char*)&troll_msg, MAXBUF+sizeof(struct sockaddr_in)+sizeof(struct sockaddr_in), 0, (struct sockaddr *)&troll_addr, troll_addr_len)) < 0){
//                                        perror("sending datagram to troll");
//                                        exit(5);
//                                }
//                        }
//
//
//                }
//        }
//
//
//        close(sock_ftpc);
//        close(sock_troll);
//        exit(0);
}
