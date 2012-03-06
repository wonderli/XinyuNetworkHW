/*client part of TCPD*/
/*connection port is 3110*/
#include "tcpd.h"
#include "troll.h"
#include "deltalist.h"
#include "crc.h"
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
void print_win()
{
        printf("\nWINDOW\n");
        int i = 0;
        for(i = 0; i < 20; i++)
        {
                printf("%d~", window[i]);
        }
}




int window_empty()
{
	int empty_flag = 0;
	int  i = 0;
	for(i = 0; i < 20; i++)
	{
		if(window[i] != -1)
			empty_flag++;
	}
	if(empty_flag == 0)
		return TRUE;
	else 
		return FALSE;

}

int main(int argc, char* argv[]) /* server program called with no argument */
{
        int sock_ftpc, ftpc_addr_len;
        int buflen = 0;
        int sock_troll, troll_addr_len;
        int sock_control, control_addr_len;
        int sock_ack, ack_addr_len;
        int sock_timer_send;
        int sock_timer_recv, timer_recv_addr_len;

        struct sockaddr_in ftpc_addr;
        struct sockaddr_in control_addr;
        //struct sockaddr_in troll_m1_addr;
        //struct sockaddr_in troll_m1_addr;
        //struct sockaddr_in troll_m2_addr;
        struct sockaddr_in troll_addr;
        struct sockaddr_in timer_send_addr;
        struct sockaddr_in timer_recv_addr;
	struct sockaddr_in ack_addr;

        fd_set read_fds;

        TCPD_MSG tcpd_msg;
	TCPD_MSG control_msg;
	TCPD_MSG ack_msg;
        //TROLL_MSG troll_msg;

        TIME_MSG timer_send;
        TIME_MSG timer_recv;
        
        struct timeval time_start, time_end, diff;
        float start, end;
	float time_rem = 0;
        int resend_pkt;
        int head = 0;
        int tail = 0;
        int ptr = 0;
        int index = 0;
	unsigned short recv_checksum;
	int i = 0;
	int j = 0;

        int new_buf_size = SOCK_BUFF_SIZE;



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
        setsockopt(sock_ftpc, SOL_SOCKET, SO_RCVBUF, &new_buf_size, sizeof(&new_buf_size));

        /*create control socket*/
        if((sock_control = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for recv from ftpc");
                exit(1);
        }
        control_addr.sin_family = AF_INET;
        control_addr.sin_port = htons(CONTROL_PORT);
        control_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if((sock_ack = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for recv from ftpc");
                exit(1);
        }
        ack_addr.sin_family = AF_INET;
        //ack_addr.sin_port = htons(TROLL_PORT_M1);
        ack_addr.sin_port = htons(TCPD_PORT_M2);
        ack_addr.sin_addr.s_addr = INADDR_ANY;
        if(bind(sock_ack, (struct sockaddr *)&ack_addr, sizeof(ack_addr)) < 0) {
                perror("ACK socket Bind failed");
                exit(2);
        }

        setsockopt(sock_ack, SOL_SOCKET, SO_RCVBUF, &new_buf_size, sizeof(&new_buf_size));

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
        timer_recv_addr.sin_addr.s_addr = INADDR_ANY;

        if(bind(sock_timer_recv, (struct sockaddr *)&timer_recv_addr, sizeof(timer_recv_addr)) < 0) {
                perror("timer recv socket Bind failed");
                exit(2);
        }
        setsockopt(sock_timer_recv, SOL_SOCKET, SO_RCVBUF, &new_buf_size, sizeof(&new_buf_size));

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
	ack_addr_len=sizeof(struct sockaddr_in);
	timer_recv_addr_len = sizeof(struct sockaddr_in);

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

        ftps_addr.sin_family = AF_INET;
        bcopy(hp->h_addr, (void*)&ftps_addr.sin_addr, hp->h_length);
        ftps_addr.sin_port = htons(TCPD_PORT_M1);



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
        for(i = 0; i < 64; i++)
        {
                buffer[i].packet.seq_num = -1;
        }
        for(i = 0; i < 20; i++)
        {
                window[i]=-1;
        }

        

        FD_ZERO(&read_fds);
        FD_SET(sock_ftpc, &read_fds);
        FD_SET(sock_timer_recv, &read_fds);
        FD_SET(sock_ack, &read_fds);
        //control_msg.packet.stop = 0;
        //sendto(sock_control, (void *)&control_msg, sizeof(TCPD_MSG), 0, (struct sockaddr *)&control_addr, sizeof(control_addr));

        while(1)
        {
                if(select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0)
                {
                        perror("SELECT ERROR");
                        exit(0);
                }
                if(FD_ISSET(sock_ftpc, &read_fds))
                {
                        //printf("\nHEAD IS %d\n", head);
                        buflen = recvfrom(sock_ftpc, (void *)&buffer[head], sizeof(TCPD_MSG), 0, (struct sockaddr *)&ftpc_addr, &ftpc_addr_len);
                        printf("\nRECVFROM FTPC: SEQ: %d\n", buffer[head].packet.seq_num);
                        print_win();
                        window[ptr] = buffer[head].packet.seq_num;
                        buffer[head].tcpd_header = ftps_addr;
                        buffer[head].checksum = cal_crc((void *)&buffer[head].packet, sizeof(struct packet_data));
                        for(i = 0; i < 64; i++)
                        {
                                if(buffer[i].packet.seq_num == window[ptr])
                                {
                                        index = i;
                                        break;
                                }

                        }
//                        printf("\nINDEX IS %d, SEQ: %d\n", index, buffer[index].packet.seq_num);

                        //index = locate_in_buffer(window[ptr]);
                        if(sendto(sock_troll, (void *)&buffer[index], sizeof(TCPD_MSG), 0, (struct sockaddr *)&troll_addr, troll_addr_len)<0)
                        {
                                perror("SEND SEQ ERROR");
                                exit(0);
                        }

                        printf("\nSENDING SEQ: %d\n", buffer[index].packet.seq_num);

			gettimeofday(&time_start, NULL);
			timer_send.seq = buffer[index].packet.seq_num;
			timer_send.time = RTO(time_rem, buffer[index].packet.seq_num);
			timer_send.action = START;
                        printf("\nSEND SEQ: %d to TIMER\n", timer_send.seq);
			//sendto(sock_timer_send, &timer_send, sizeof(TIME_MSG), 0, (struct sockaddr *)&timer_send_addr, sizeof(timer_send_addr));//send to timer
			
			ptr++;//move window index
			head = (head + 1) % 64;//wrap buffer
			if(ptr >= 19)
			{
				printf("\nWINDOW FULL, SLEEP\n");
				control_msg.packet.stop = 1;
				sendto(sock_control, (void *)&control_msg, sizeof(TCPD_MSG), 0, (struct sockaddr *)&control_addr, sizeof(control_addr));
			}
			else
			{
				printf("\nWINDOW NOT FULL, KEEP SENDING\n");
				control_msg.packet.stop = 0;
				sendto(sock_control, (void *)&control_msg, sizeof(TCPD_MSG), 0, (struct sockaddr *)&control_addr, sizeof(control_addr));
			}


                }//END SOCK_FTPC

		// IF PACKET RECEIVED or RETRANSMISSION
		if(FD_ISSET(sock_ack, &read_fds))
		{
                        printf("\nGET ACK\n");
			gettimeofday(&time_end, NULL);
			time_rem = RTT(&time_start, &time_end);

			recvfrom(sock_ack, (void*)&ack_msg, sizeof(TCPD_MSG),0, (struct sockaddr *)&ack_addr, &ack_addr_len);
			recv_checksum = cal_crc((void*)&ack_msg.packet, sizeof(struct packet_data));
                        //printf("\nCAL CHECKSUM: %lu, RECV CHECKSUM: %lu\n", recv_checksum, ack_msg.checksum);
                        printf("\nACK SEQ: %d\n", ack_msg.packet.ack_seq);

			if(ack_msg.checksum == recv_checksum)
			{
				if(ack_msg.packet.ack == 1)
				{

					//Delete this seq from Timer
					timer_send.seq = ack_msg.packet.ack_seq;
					timer_send.action = CANCEL;
					timer_send.time = 0;//Don't know whether this will work or not
                                        printf("\nCANCEL TIMER\n");
					//sendto(sock_timer_send, &timer_send, sizeof(timer_send), 0, (struct sockaddr *) &timer_send_addr, sizeof(timer_send_addr));

					for(i = 0; i < 20; i++)
					{
						if(window[i] == ack_msg.packet.ack_seq);
						{
							window[i] = -1;//RECV ACK
                                                        printf("I've cleaned window: windoes[%d]\n, SEQ: %d", i, ack_msg.packet.ack_seq);
                                        //                break;
                                                        print_win();
						}
					}
                                        printf("\nAFTER RECV ACK THE WIN IS:");
                                        print_win();

                                        //control_msg.packet.stop = 0;//WINDOW NOT FULL, KEEP SENDING
                                        //sendto(sock_control, (void *)&control_msg, sizeof(TCPD_MSG), 0, (struct sockaddr *)&control_addr, sizeof(control_addr));
					if(window_empty() == TRUE)
					{
                                                printf("\nWINDOW EMPTY\n");
						control_msg.packet.stop = 0;//WINDOW EMPTY, KEEP SENDING
						ptr = 0;//Make pointer back to begin
						sendto(sock_control, (void *)&control_msg, sizeof(TCPD_MSG), 0, (struct sockaddr *)&control_addr, sizeof(control_addr));
					}
				}
				if(ack_msg.packet.fin_ack == 1)
				{
					printf("\nREACHED TO FINISH\n");
					recvfrom(sock_ack, (void *)&ack_msg, sizeof(ack_msg), 0, (struct sockaddr *)&ack_msg, &ack_addr_len);//WAITING FOR FIN ACK
					if(strcmp(ack_msg.packet.data, "FIN"))
					{
						control_msg.packet.stop = 1;
						sendto(sock_control, (void*)&control_msg, sizeof(TCPD_MSG), 0, (struct sockaddr *)&control_addr, sizeof(control_addr));

						timer_send.seq = ack_msg.packet.ack_seq;
						timer_send.action = CANCEL;
						timer_send.time = 0;
                                                printf("\n CANCEL NODE SEQ: %d\n", timer_send.seq);
						//sendto(sock_timer_send, &timer_send, sizeof(timer_send), 0, (struct sockaddr *) &timer_send_addr, sizeof(timer_send_addr));
						close(sock_troll);
						close(sock_ftpc);
						exit(0);
					}
					
				}//END ACKMSG FIN_ACK
			}//END ACKMSG CHECKSUM

			// IF PACKET EXPIRED
			if(FD_ISSET(sock_timer_recv, &read_fds))
			{
				
				if(recvfrom(sock_timer_recv, &timer_recv, sizeof(timer_recv), 0, (struct sockaddr *)&timer_recv_addr, &timer_recv_addr_len) > 0)
				{
					printf("\n PACKET SEQ NUM: %d HAS EXPIRED\n", timer_recv.seq);

				}
				for(i = 0; i < 20; i++)//ITERATE WINDOW
				{
					if(window[i] == timer_recv.seq)
					{
						for(j = 0; j < 64; j++)
						{

							if((buffer[j].packet.seq_num == timer_recv.seq) && (buffer[j].packet.seq_num != -1))
							{
								printf("\nRESEND TO BUFFER\n");
								resend_pkt = j;
							}
						}//END
					}//END if window
				}//END ITERATE

				sendto(sock_troll, (void *)&buffer[resend_pkt], sizeof(TCPD_MSG), 0, (struct sockaddr *)&troll_addr, sizeof(troll_addr));

				gettimeofday(&time_start, NULL);
				timer_send.time = RTO(time_rem, buffer[resend_pkt].packet.seq_num);
				timer_send.seq = buffer[resend_pkt].packet.seq_num;
				timer_send.action = START;
				sendto(sock_timer_send, &timer_send, sizeof(timer_send), 0, (struct sockaddr*)&timer_send_addr, sizeof(timer_send_addr));
			}
		}
		FD_ZERO(&read_fds);
		FD_SET(sock_ftpc, &read_fds);
		FD_SET(sock_timer_recv, &read_fds);
		FD_SET(sock_ack, &read_fds);
        }
}
