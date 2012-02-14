#include "deltalist.h"
/*
struct timeval timeout = {
        1,
        0,
};
*/
int main()
{
        int sock_timer;
        int timer_addr_len;
        TIME_MSG time_msg_recv;
//        char timer_buf[sizeof(TIME_MSG)];
        TIME_MSG time_msg_send;
        struct sockaddr_in timer_addr;
//        struct sockaddr_in timer_addr;
        linklist *time_list;
        if((sock_timer = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
        {
                perror("opening datagram socket for recv from tcpd_m1");
        }
        timer_addr.sin_family = AF_INET;
        timer_addr.sin_port = htons(TIMER_PORT);
        timer_addr.sin_addr.s_addr = INADDR_ANY;

        printf("PORT IS %d\n", timer_addr.sin_port);


        if(bind(sock_timer, (struct sockaddr *)&timer_addr, sizeof(timer_addr)) < 0)
        {
                perror("RECV(receive from tcpd_m2) socket Bind failed");
                exit(0);
        }

        time_list = create_list();
        fd_set fd_read_set;
        struct timeval tv1, tv2;
        struct timezone tz;
        int time_set_flag = 0;
        int len = sizeof(timer_addr);
        int MAXFD = sock_timer + 1;
        while(1)
        { 
                FD_ZERO(&fd_read_set);
                FD_SET(sock_timer, &fd_read_set);

                if(time_set_flag == 0)
                {
                        gettimeofday(&tv1, &tz);
                }                
                if(select(MAXFD, &fd_read_set, NULL, NULL, NULL) < 0)
                {
                        perror("select error");
                        exit(0);
                }
                if(FD_ISSET(sock_timer, &fd_read_set))
                {
                        if(recvfrom(sock_timer, (char *)&time_msg_recv, sizeof(time_msg_recv), 0, (struct sockaddr*)&timer_addr, &len) < 0)
                        {
                                perror("recvfrom TCPD_M2 error");
                                exit(0);
                        }
                        if(time_msg_recv.action == CANCEL)
                        {
                                printf("cancel node\n");
                                cancel_node(time_list, time_msg_recv.seq);
                                print_list(time_list);
                        }else if(time_msg_recv.action == START)
                        {
                                printf("start node\n");
                                node *new_node = creat_node(time_msg_recv.seq, time_msg_recv.time);
                                insert_node(time_list, new_node);
                                print_list(time_list);
                        }
                        time_set_flag = 1;
                        continue;
                }
                time_set_flag = 0;
//                print_list(time_list);
                if(time_list->len > 0)
                {
                        gettimeofday(&tv2, &tz);
                        usleep(1e6 - (tv2.tv_usec - tv1.tv_usec));
                        time_list->head->time = 1e6 - (tv2.tv_usec - tv1.tv_usec);
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
//                        FD_ZERO(&fd_read_set);
//                        FD_SET(sock_timer, &fd_read_set);
                        print_list(time_list);
                }
        }
        close(sock_timer);
        return 0;

}
