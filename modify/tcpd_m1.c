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
//int find_min()
//{
//        int min=10000;
//        int i = 0;
//        for(i = 0;i < 20;i++)
//        {
//                if(window_srv[i]<min && window_srv[i] != -1)
//                {
//                        min = window_srv[i];
//                }
//
//        }
//        return min;
//}
//int find_min_buffer(int min)
//{
//        int i = 0;
//        for(i = 0; i < 64; i++)
//        {
//                if(recv_buffer[i].packet.seq_num == min)
//                {
//                        return i;
//                }
//        }
//
//}
//int find_min_win_index(int min)
//{
//        int index;
//        int i = 0;
//        for(i=0; i < 20; i++)
//        {
//                if(window_srv[i] == min && window_srv[i] != -1)
//                {
//                        index = i;
//                }
//        }
//        return index;
//
//}
void print_win()
{
        printf("\nWINDOW\n");
        int i = 0;
        for(i = 0; i < 20; i++)
        {
                printf("%d~", window_srv[i]);
        }
}
int main(int argc, char* argv[]) /* server program called with no argument */
{
        int sock_ftps, ftps_addr_len;
	int sock_from_troll_m2, from_troll_addr_len;
	int sock_ack, ack_addr_len;
	unsigned long checksum = 0;

        int buflen = 0;
	int head = 0;
//	int tail = 0;
	
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
	int lastsent = -1;
        fd_set read_fds;
	
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
        troll_m2_addr.sin_port = htons(TCPD_PORT_M1);
        troll_m2_addr.sin_addr.s_addr = INADDR_ANY;        

        if(bind(sock_from_troll_m2, (struct sockaddr *)&troll_m2_addr, sizeof(troll_m2_addr)) < 0) {
                perror("Recv(receive from troll socket Bind failed");
                exit(2);
        }

	if((sock_ack = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("opening datagram socket for send ack to tcpd_m2");
		exit(1);
	}

	troll_m1_addr.sin_family = AF_INET;
	troll_m1_addr.sin_port = htons(TROLL_PORT_M1);
	troll_m1_addr.sin_addr.s_addr = inet_addr("127.0.0.1");     

	//Contruct troll header
	ack_addr.sin_family = AF_INET;
	ack_addr.sin_port = htons(TCPD_PORT_M2);
	bcopy(hp->h_addr, (void*)&ack_addr.sin_addr, hp->h_length);

        ftps_addr.sin_family = AF_INET;
	ftps_addr.sin_port = htons(TCPD_PORT);
	ftps_addr.sin_addr.s_addr = inet_addr("127.0.0.1");     

        //GET LEN FOR RECVFROM
	ftps_addr_len=sizeof(struct sockaddr_in);
	from_troll_addr_len=sizeof(struct sockaddr_in);
	ack_addr_len  = sizeof(struct sockaddr_in);
        

       // recvfrom(sock_from_troll_m2, (void *)&recv_buffer[head], sizeof(TCPD_MSG), 0, (struct sockaddr *)&troll_m2_addr, &from_troll_addr_len);
        FD_ZERO(&read_fds);
        FD_SET(sock_from_troll_m2, &read_fds);

        while(1)
        {
                if(select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0)
                {
                        perror("SELECT ERROR");
                        exit(0);
                }
                if(FD_ISSET(sock_from_troll_m2, &read_fds))
                {

                        recvfrom(sock_from_troll_m2, (void *)&recv_buffer[head], sizeof(TCPD_MSG), 0, (struct sockaddr *)&troll_m2_addr, &from_troll_addr_len);
                        printf("\nRECV FROM TROLL_M2, SEQ:%d\n", recv_buffer[head].packet.seq_num);

                        checksum = cal_crc((void *)&recv_buffer[head].packet, sizeof(struct packet_data));//CRC

                        printf("\nCAL CHECKSUM: %lu, RECV CHECKSUM: %lu\n", checksum, recv_buffer[head].checksum);
                        if(checksum == recv_buffer[head].checksum)
                        {
                                crc_match = TRUE;
                                ack_buffer_flag = FALSE;//CHECK FOR DUP

                                for(i = 0; i< 64; i++)
                                {
                                        if(ack_buffer[i] == recv_buffer[head].packet.seq_num)
                                        {
                                                ack_buffer_flag = TRUE;
                                        }
                                }
                                //printf("\nACK_BUFFER_FLAG STATUS %d\n", ack_buffer_flag);

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
                                        }      

                                        if(ack_exist == TRUE)
                                        {
                                                printf("\nIN THE WINDOWS, WILL ARRIVE FTPS SOON\n",recv_buffer[head].packet.seq_num);
                                        }
                                        else if(ack_exist == FALSE)//NOT IN WINDOW
                                        {
                                                //window_index = recv_buffer[head].packet.seq_num % 20;
                                                window_index = recv_buffer[head].packet.seq_num;
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
                                }//end if ack_flag
                                else if(ack_buffer_flag == TRUE)
                                {
                                        printf("\nACK FOR DUPLICATE AGAIN\n");
                                        ack.packet.ack = 1;
                                        ack.packet.ack_seq = recv_buffer[head].packet.seq_num;
                                        ack.tcpd_header = ack_addr;
                                        ack.checksum = cal_crc((void *)&ack.packet, sizeof(struct packet_data));
                                        //sendto(sock_ack, (void *)&ack, sizeof(TCPD_MSG), 0, (struct sockaddr *)&ack_addr, sizeof(ack_addr));
                                        sendto(sock_ack, (void *)&ack, sizeof(TCPD_MSG), 0, (struct sockaddr *)&troll_m1_addr, sizeof(troll_m1_addr));
                                        printf("\nSEND ACK SEQ %d, TO TROLL M1\n", ack.packet.ack_seq);
                                }
                }
                else
                {
                        crc_match = FALSE;//checksum wrong
                }

                if(crc_match == TRUE)
                {
                        printf("\nENTER IF CRC_MATCH\n");
                        //find the lowest window seq;
                        lowest_seq = 100000;
                        for(i = 0; i < 20; i++)
                        {
                                if(window_srv[i] < lowest_seq && window_srv[i] != -1)
                                {
                                        lowest_seq = window_srv[i];
                                        lowest_seq_window_index = i;
                                }
                        }
//                        lowest_seq = find_min();
//                        lowest_seq_window_index = find_min_win_index(lowest_seq);
                        printf("\nLOWEST_SEQ %d\n", lowest_seq);
                        printf("\nLASTSENT+1: %d\n", lastsent+1);
                        print_win();
                        if(lowest_seq == (lastsent + 1))//if lowest in win is to be sent
                        {
                                int buffer_index = 0;
                                for(i = 0; i < 64; i++)
                                {
                                        if(recv_buffer[i].packet.seq_num == lowest_seq)
                                        {
                                                buffer_index = i;
                                                break;
                                        }
                                }

 //                               int buffer_index = find_min_buffer(lowest_seq);
                                sendto(sock_ftps, (void *)&recv_buffer[buffer_index], sizeof(TCPD_MSG), 0, (struct sockaddr *)&ftps_addr, sizeof(ftps_addr));
                                printf("\nSEND SEQ:%d to ftps\n", recv_buffer[buffer_index].packet.seq_num);

                                lastsent = recv_buffer[buffer_index].packet.seq_num;
                                if(recv_buffer[buffer_index].packet.fin != 1)
                                {
                                        ack.packet.ack = 1;
                                        ack.packet.ack_seq = recv_buffer[buffer_index].packet.seq_num;
                                        //lastsent = recv_buffer[buffer_index].packet.seq_num;
                                        printf("\nLAST SENT: %d\n", lastsent);
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
					printf("\nACK CHECKSUM %d\n", ack.checksum);
                                        ack.tcpd_header = ack_addr; 
                                        //sendto(sock_ack, (void *)&ack, sizeof(TCPD_MSG), 0, (struct sockaddr *)&ack_addr, sizeof(ack_addr));
                                        sendto(sock_ack, (void *)&ack, sizeof(TCPD_MSG), 0, (struct sockaddr *)&troll_m1_addr, sizeof(troll_m1_addr));
                                        window_srv[lowest_seq_window_index] = -1;
                                }//end if fin != 1
                                else if(recv_buffer[buffer_index].packet.fin == 1)
                                {
                                        printf("\nRECEIVE FIN!!!\n");
                                        sendto(sock_ftps, (void*)&recv_buffer[buffer_index], sizeof(TCPD_MSG), 0, (struct sockaddr *)&ftps_addr, sizeof(ftps_addr));
                                        //sendto(sock_ftps, (void*)&recv_buffer[buffer_index], sizeof(TCPD_MSG), 0, (struct sockaddr *)&ftps_addr, sizeof(ftps_addr));
                                        ack.tcpd_header = ack_addr;
                                        ack.packet.fin_ack = 1;
                                        ack.packet.ack = 0;
                                        ack.packet.ack_seq = recv_buffer[buffer_index].packet.seq_num;
                                        ack.checksum = cal_crc((void *)&ack.packet, sizeof(struct packet_data));
                                        window_srv[lowest_seq_window_index] = -1;
                                        strcpy(ack.packet.data, "FIN");
                                        //sendto(sock_ack, (void *)&ack, sizeof(TCPD_MSG), 0, (struct sockaddr *)&ack_addr, sizeof(ack_addr));
                                        sendto(sock_ack, (void *)&ack, sizeof(TCPD_MSG), 0, (struct sockaddr *)&troll_m1_addr, sizeof(troll_m1_addr));
                                        printf("\nFINISH TRANSFER FILE\n");
                                        close(sock_ack);
                                        close(sock_from_troll_m2);
                                        close(sock_ftps);
                                        exit(0);
                                }
                        }//END IF LAST SENT
                        else
                        {
                                printf("\nSLEEP FOR WAITING\n");
                                usleep(100000);
                        }//END LOW
                }
                else if(crc_match == FALSE)
                {
                        printf("\nCRC WRONG, RETRANSMIT\n");
                }
           }
           FD_ZERO(&read_fds);
           FD_SET(sock_from_troll_m2,&read_fds);
      }

}

