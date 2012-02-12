#include "deltalist.h"
struct timeval timeout = {
        0,
        0,
};
int main()
{
        int sock_timer;
        int timer_addr_len;
        TIME_MSG time_msg_recv;
//        char timer_buf[sizeof(TIME_MSG)];
        TIME_MSG time_msg_send;
        struct sockaddr_in timer_addr;
        struct sockaddr_in tcpd_m2_addr;
        linklist *time_list;
        if((sock_timer = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
        {
                perror("opening datagram socket for recv from tcpd_m1");
        }
        tcpd_m2_addr.sin_family = AF_INET;
        tcpd_m2_addr.sin_port = htons(TCPD_PORT);
        tcpd_m2_addr.sin_addr.s_addr = INADDR_ANY;
        if(bind(sock_timer, (struct sockaddr *)&tcpd_m2_addr, sizeof(tcpd_m2_addr)) < 0)
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
        int len = sizeof(tcpd_m2_addr);
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
                        if(recvfrom(sock_timer, (char *)&time_msg_recv, sizeof(time_msg_recv), 0, (struct sockaddr*)&tcpd_m2_addr, &len) < 0)
                        {
                                perror("recvfrom TCPD_M2 error");
                                exit(0);
                        }
                        if(time_msg_recv.action == CANCEL)
                        {
                                cancel_node(time_list, time_msg_recv.seq);
                        }else if(time_msg_recv.action == START)
                        {
                                node *cancel_node = creat_node(time_msg_recv.seq, time_msg_recv.time);
                                insert_node(time_list, cancel_node);
                        }
                        time_set_flag = 1;
                        continue;
                }
                time_set_flag = 0;
                if(time_list->len > 0)
                {
                        gettimeofday(&tv2, &tz);
                        usleep(100000 - (tv2.tv_usec - tv1.tv_usec));
                        time_list->head->time = time_list->head->time - (tv2.tv_usec - tv1.tv_usec);
                        if(expire(time_list) == TRUE)
                        {
                                gettimeofday(&tv2, &tz);
                                node *expire_node, *ptr;
                                ptr = time_list->head;
                                while(ptr && ptr->time <= 0)
                                {
                                        expire_node = ptr;
                                        ptr = ptr->next;
                                        time_msg_send.seq = expire_node->seq;
                                        time_msg_send.action = EXPIRE;
                                        remove_node(expire_node);
                                        time_list->len--;
                                }
                                time_list->head = ptr;
                                if(time_list->head == NULL)
                                {
                                        time_list->tail == NULL;
                                }
                        }
                        FD_ZERO(&fd_read_set);
                        FD_SET(sock_timer, &fd_read_set);
                }
        }
        close(sock_timer);
        return 0;

}
