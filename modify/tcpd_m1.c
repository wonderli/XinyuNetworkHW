/*client part of TCPD*/
/*connection port is 3110*/
#include "deltalist.h"
#include "tcpd.h"
#include "troll.h"
#include "crc.h"
int window_srv[20];
int ptr = 0;
int ack_buffer[64];
TCPD_MSG recv_buffer[64];
int main(int argc, char* argv[]) /* server program called with no argument */
{
        int sock_ftps, ftps_addr_len;
	int sock_from_troll_m2, from_troll_addr_len;
	int sock_ack, ack_addr_len;
	unsigned long checksum = 0;

        int buflen = 0;
	int head = 0;
	int tail = 0;
	
        struct sockaddr_in ftps_addr;
        struct sockaddr_in troll_m2_addr;
	struct sockaddr_in troll_m1_addr;
	struct sockaddr_in ack_addr;

	struct hostent *hp;
	hp = gethostbyname(argv[1]);
	if(hp == 0)
	{
		perror("Unknown host");
		exit(1);
	}

        TCPD_MSG tcpd_recv;
	TCPD_MSG ack;
	TCPD_MSG tcpd_send;

	int i = 0;
	int j = 0;
        //FLAGS
	int crc_match = FALSE;
	int ack_buffer_flag = FALSE;
	int ack_exist = FALSE;
	int window_index = 0;


	int lowest_seq = 100000; 
	int lowest_seq_window_index = 0;
	int lastsent;
	
	//init window
	for(i = 0; i < 20; i++)
	{
		window_srv[i] = -1;
	}
	//init ack
	for(j = 0; j < 64; j++)
	{
		ack_buffer[j] = -1;
	}
	
        /*create from ftps socket*/
        if((sock_ftps = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for send to ftps");
                exit(1);
        }

        /*create troll socket*/
        if((sock_from_troll_m2 = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("opening datagram socket for recv from troll m2");
                exit(1);
        }

        /* create troll_addr with parameters and bind troll_addr to socket */
        troll_m2_addr.sin_family = AF_INET;
        troll_m2_addr.sin_port = htons(TROLL_PORT_M1);
        troll_m2_addr.sin_addr.s_addr = INADDR_ANY;        

        if(bind(sock_from_troll_m2, (struct sockaddr *)&troll_m2_addr, sizeof(troll_m2_addr)) < 0) {
                perror("Recv(receive from troll) socket Bind failed");
                exit(2);
        }

	if((sock_ack = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("opening datagram socket for send ack to tcpd_m2");
		exit(1);
	}

	troll_m1_addr.sin_family = AF_INET;
	troll_m1_addr.sin_port = htons(TROLL_PORT_M2);
	troll_m1_addr.sin_addr.s_addr = inet_addr("127.0.0.1");     

	//Contruct troll header
	ack_addr.sin_family = AF_INET;
	ack_addr.sin_port = htons(TCPD_PORT_M2);
	bcopy(hp->h_addr, (void*)&ack_addr.sin_addr, hp->h_length);

        //GET LEN FOR RECVFROM
	ftps_addr_len=sizeof(struct sockaddr_in);
	from_troll_addr_len=sizeof(struct sockaddr_in);
	ack_addr_len  = sizeof(struct sockaddr_in);

	while(1)
	{
		recvfrom(sock_from_troll_m2, (void *)&recv_buffer[head], sizeof(TCPD_MSG), 0, (struct sockaddr *)&troll_m2_addr, &from_troll_addr_len);

		checksum = cal_crc((void *)&recv_buffer[head].packet, sizeof(struct packet_data));//CRC

		if(checksum == recv_buffer[head].checksum)
		{
			crc_match = TRUE;
			ack_buffer_flag = FALSE;

			for(i = 0; i< 64; i++)
			{
				if(ack_buffer[i] == recv_buffer[head].packet.seq_num)
				{
					ack_buffer_flag = TRUE;
				}
			}

			if(ack_buffer_flag != TRUE)//NOT IN BUFFER
			{
				ack_exist = FALSE;

				for(i = 0; i < 20; i++)
				{
                                        //CHECK WHETHER IN WINOW OR NOT
					if(window_srv[i] == recv_buffer[head].packet.seq_num)
					{
						ack_exist = TRUE;
					}

					if(ack_exist == TRUE)
					{
						printf("\nIN THE WINDOWS, WILL ARRIVE\n",recv_buffer[head].packet.seq_num);
					}
					else if(ack_exist == FALSE)//NOT IN WINDOW
					{
						window_index = recv_buffer[head].packet.seq_num % 20;
						window_srv[window_index] = recv_buffer[head].packet.seq_num;

						if(head < 63)
						{
							head++;
						}
						else
						{
							head = 0;//Make buffer back

						}
					}
				}
			}//end if ack_flag
			else if(ack_buffer_flag == TRUE)
			{
				ack.packet.ack = 1;
				ack.packet.ack_seq = recv_buffer[head].packet.seq_num;
				ack.tcpd_header = ack_addr;
				ack.checksum = cal_crc((void *)&ack.packet, sizeof(struct packet_data));
				sendto(sock_ack, (void *)&ack, sizeof(TCPD_MSG), 0, (struct sockaddr *)&ack_addr, sizeof(ack_addr));
			}
		}

		if(crc_match == TRUE)
		{
			//find the lowest window seq;
			for(i = 0; i < 20; i++)
			{
				if(window_srv[i] < lowest_seq && window_srv[i] != -1)
				{
					lowest_seq = window_srv[i];
					lowest_seq_window_index = i;
				}
			}
			if(lowest_seq == (lastsent + 1))//if lowest in win is to be sent
			{
				int buffer_index = 0;
				for(i = tail; i < 64; i++)
				{
					if(recv_buffer[i].packet.seq_num == lowest_seq)
					{
						buffer_index = i;
					}
				}
				sendto(sock_ftps, (void *)&recv_buffer[buffer_index], sizeof(TCPD_MSG), 0, (struct sockaddr *)&ftps_addr, sizeof(ftps_addr));
				if(recv_buffer[buffer_index].packet.fin != 1)
				{
					ack.packet.ack = 1;
					ack.packet.ack_seq = recv_buffer[buffer_index].packet.seq_num;
					lastsent = recv_buffer[buffer_index].packet.seq_num;
					ack_buffer[ptr] = recv_buffer[buffer_index].packet.seq_num;
					if(ptr < 63)
					{
						ptr++;
					}
					else
					{
						ptr = 0;
					}
					ack.checksum = cal_crc((void*)&ack.packet, sizeof(struct packet_data));
					ack.tcpd_header = ack_addr; 
					sendto(sock_ack, (void *)&ack, sizeof(TCPD_MSG), 0, (struct sockaddr *)&ack_addr, sizeof(ack_addr));
					window_srv[lowest_seq_window_index] = -1;
				}//end if fin != 1
				else if(recv_buffer[buffer_index].packet.fin == 1)
				{
					printf("\nRECEIVE FIN!!!\n");
					sendto(sock_ftps, (void*)&recv_buffer[buffer_index], sizeof(TCPD_MSG), 0, (struct sockaddr *)&ftps_addr, sizeof(ftps_addr));
					ack.tcpd_header = ack_addr;
					ack.packet.fin_ack = 1;
					ack.packet.ack = 0;
					ack.packet.ack_seq = recv_buffer[buffer_index].packet.seq_num;
					ack.checksum = cal_crc((void *)&ack.packet, sizeof(struct packet_data));
					window_srv[lowest_seq_window_index] = -1;
					strcpy(ack.packet.data, "FIN");
					sendto(sock_ack, (void *)&ack, sizeof(TCPD_MSG), 0, (struct sockaddr *)&ack_addr, sizeof(ack_addr));
					printf("\nFINISH TRANSFER FILE\n");
					close(sock_ack);
					close(sock_from_troll_m2);
					close(sock_ftps);
					exit(0);
				}
			}
			else
			{
				printf("\nSLEEP FOR WAITING\n");
				usleep(1000000);
			}//END LOW
		}
		else if(crc_match == FALSE)
		{
			printf("\nCRC WRONG, RETRANSMIT\n");
		}
	}

}