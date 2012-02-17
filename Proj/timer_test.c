#include "deltalist.h"
int main(int argc, char* argv[])
{
        int sock;
        struct sockaddr_in sin_addr;
        struct hostent *hp;
        if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
        {
                perror("UDP socket opening error");
                exit(1);                
        }
        hp = gethostbyname("127.0.0.1");
        if(hp == 0)
        {
                perror("host unkonwn");
                exit(2);
        }
        bcopy((void *)hp->h_addr, (void *)&sin_addr.sin_addr, hp->h_length);
        sin_addr.sin_family = AF_INET;
        sin_addr.sin_port = htons(TIMER_PORT);
        printf("PORT IS: %d\n", sin_addr.sin_port);


        TIME_MSG time_msg_send;
        TIME_MSG time_msg_recv;

        /* Creat new packet*/
        time_msg_send.seq = 1;
        time_msg_send.time = 2*1e6;
        time_msg_send.action = START;
        

        /* Send msg to timer*/
        if(sendto(sock, (char *)&time_msg_send, sizeof(TIME_MSG),0, (struct sockaddr*)&sin_addr, sizeof(sin_addr)) < 0)
        {
                perror("send msg to time error");
                exit(3);        
        }

        time_msg_send.seq = 2;
        time_msg_send.time = 3*1e6;
        time_msg_send.action = START;

        if(sendto(sock, (char *)&time_msg_send, sizeof(TIME_MSG),0, (struct sockaddr*)&sin_addr, sizeof(sin_addr)) < 0)
        {
                perror("send msg to time error");
                exit(3);        
        }

        time_msg_send.seq = 3;
        time_msg_send.time = 4*1e6;
        time_msg_send.action = START;

        if(sendto(sock, (char *)&time_msg_send, sizeof(TIME_MSG),0, (struct sockaddr*)&sin_addr, sizeof(sin_addr)) < 0)
        {
                perror("send msg to time error");
                exit(3);        

        }
        usleep(5*1e6);

        time_msg_send.seq = 3;
        time_msg_send.time = 0;
        time_msg_send.action = CANCEL;

        if(sendto(sock, (char *)&time_msg_send, sizeof(TIME_MSG),0, (struct sockaddr*)&sin_addr, sizeof(sin_addr)) < 0)
        {
                perror("send msg to time error");
                exit(3);        
        }

        time_msg_send.seq = 4;
        time_msg_send.time = 10*1e6;
        time_msg_send.action = START;

        if(sendto(sock, (char *)&time_msg_send, sizeof(TIME_MSG),0, (struct sockaddr*)&sin_addr, sizeof(sin_addr)) < 0)
        {
                perror("send msg to time error");
                exit(3);        
        }
        usleep(5*1e6);

        time_msg_send.seq = 4;
        time_msg_send.time = 5*1e6;
        time_msg_send.action = CANCEL;

        if(sendto(sock, (char *)&time_msg_send, sizeof(TIME_MSG),0, (struct sockaddr*)&sin_addr, sizeof(sin_addr)) < 0)
        {
                perror("send msg to time error");
                exit(3);        
        }
        usleep(5*1e6);


}

