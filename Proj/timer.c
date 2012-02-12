#include "deltalist.h"
int main()
{
        int sock_timer;
        int timer_addr_len;
        char timer_buf[sizeof(time_msg)];
        TIME_MSG time_msg ;
        struct sockaddr_in timer_addr;
        struct sockaddr_in tcpd_m2_addr;
        list *time_list;
        if((sock_timer = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
        {
                perror("opening datagram socket for recv from tcpd_m1");
        }
        tcpd_m2_addr.sin_family = AF_INET;
        tcpd_m2_addr.sin_port = htons(TCPD_PORT);
        tcpd_m2_addr.sin_addr.s_addr = INADDR_ANY;
        if(bind(sock_timer, (struct sockaddr *)&tcpd_m2_addr, siezof(tcpd_m2_addr)) < 0)
        {
                perror("RECV(receive from tcpd_m2) socket Bind failed");
                exit(0);
        }
        time_list = create_list();
        fd_set fd_read_set;
        FD_ZERO(&fd_read_set);
        FD_SET(sock_timer, &fd_read_set);
        struct timeval tv1, tv2;
        struct timezone tz;
        int time_set_flag = 0;
        while(1)
        {
                if(time_set_flag == 0)
                {
                        gettimeofday(&tv1, &tz);
                }                
                if(select(FD_SETSIZE, &fd_read_set, NULL, NULL, &timeout))
                {
                        perror("select error");
                        exit(0);
                }
                if(FD_ISSET(sock_timer, &fd_read_set))
                {
                        if(recvfrom(sock_timer, (char *)&time_msg, sizeof(time_msg), 0, (struct sockaddr*)&tcpd_m2_addr, &len) < 0)
                        {
                                perror("recvfrom TCPD_M2 error");
                                exit(0);
                        }
                        if(time_msg.opt == CANCEL)
                        {
                                cancel_node(time_list, time_msg.seq);
                        }else if(time_msg.opt == START)
                        {
                                node *cancel_node = creat_node(time_msg.seq, time_msg.time);
                                insert_node(time_list, cancel_node);
                        }
                        time_set_flag = 1;
                        continue;
                }
                time_set_flag = 0;
                if(list->len > 0)
                {
                        gettimeofday(&tv2, &tz);
                        usleep(100000)

                }




        }



}
