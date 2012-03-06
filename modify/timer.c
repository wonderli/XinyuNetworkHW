#include "deltalist.h"

struct timeval timeout = {
        0,
        1*1e5,
};

int main()
{
        //int sock_timer;
        int sock_timer_recv;
	int sock_timer_send;
        int timer_addr_len;
        TIME_MSG time_msg_recv;
//        char timer_buf[sizeof(TIME_MSG)];
        TIME_MSG time_msg_send;
        struct sockaddr_in timer_recv_addr;
        struct sockaddr_in timer_send_addr;
        int new_buf_size = SOCK_BUFF_SIZE;
//        struct sockaddr_in timer_addr;
        linklist *time_list;
        if((sock_timer_recv = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
        {
                perror("opening datagram socket for recv from tcpd_m1");
        }
        timer_recv_addr.sin_family = AF_INET;
        timer_recv_addr.sin_port = htons(TIMER_SEND_PORT);
        timer_recv_addr.sin_addr.s_addr = INADDR_ANY;
	
//        printf("PORT IS %d\n", timer_addr.sin_port);


        if(bind(sock_timer_recv, (struct sockaddr *)&timer_recv_addr, sizeof(timer_recv_addr)) < 0)
        {
                perror("Timer send to tcpd socket Bind failed");
                exit(0);
        }
        setsockopt(sock_timer_recv, SOL_SOCKET, SO_RCVBUF, &new_buf_size, sizeof(&new_buf_size));

	if((sock_timer_send = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
        {
                perror("sock_timer_send sock failed )");
        }
        timer_send_addr.sin_family = AF_INET;
        timer_send_addr.sin_port = htons(TIMER_RECV_PORT);
        timer_send_addr.sin_addr.s_addr = inet_addr("127.0.0.1");



        time_list = create_list();
        fd_set fd_read_set;
        struct timeval tv1, tv2;
        struct timezone tz;
        int time_set_flag = 0;
        int len = sizeof(timer_recv_addr);
        int MAXFD = sock_timer_recv + 1;
        FD_ZERO(&fd_read_set);
        FD_SET(sock_timer_recv, &fd_read_set);

        while(1)
        { 

                if(time_set_flag == 0)
                {
                        gettimeofday(&tv1, &tz);
                }
                /* Receive msg from socket*/
                if(select(MAXFD, &fd_read_set, NULL, NULL, &timeout) < 0)
                {
                        perror("select error");
                        exit(0);
                }
                if(FD_ISSET(sock_timer_recv, &fd_read_set))
                {
                        if(recvfrom(sock_timer_recv, (char *)&time_msg_recv, sizeof(time_msg_recv), 0, (struct sockaddr*)&timer_recv_addr, &len) < 0)
                        {
                                perror("recvfrom TCPD_M2 error");
                                exit(0);
                        }
                        if(time_msg_recv.action == CANCEL) /* Cancel node*/
                        {
                                printf("\ncancel node\n");
                                cancel_node(time_list, time_msg_recv.seq);
                                print_list(time_list);
                        }else if(time_msg_recv.action == START) /* Add node for timing */
                        {
                                printf("\nstart node\n");
                                node *new_node = creat_node(time_msg_recv.seq, time_msg_recv.time);
                                insert_node(time_list, new_node);
                                print_list(time_list);
                        }
                        time_set_flag = 1;
                        continue;
                }
                time_set_flag = 0;
                timeout.tv_sec = 0;
//                timeout.tv_usec = 1*1e5;
                timeout.tv_usec = 0;
                if(time_list->len > 0)
                {
                        gettimeofday(&tv2, &tz);
                        usleep(1000000 - (tv2.tv_usec - tv1.tv_usec));
                        /* Update data*/
                        time_list->head->time = time_list->head->time - (1000000 - (tv2.tv_usec - tv1.tv_usec));                        
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
                                        time_msg_send.time = 0;
                                        printf("\nBEGIN REMOVE NODE\n");
                                        remove_node(expire_node);
                                        time_list->len--;
                                        again:
                                        if(sendto(sock_timer_send, (void *)&time_msg_send, sizeof(time_msg_send), 0, (struct sockaddr*)&timer_send_addr, sizeof(struct sockaddr_in)) < 0);
                                        {
                                               if(errno == EINTR) goto again;
                                                perror("\nTIMER SEND ERROR\n");
                                                exit(1);
                                        }
                                        printf("\nREMOVE NODE\n");
                                }
                                time_list->head = ptr;
                                if(time_list->head == NULL)
                                {
                                        time_list->tail = NULL;
                                }
                                print_list(time_list);
                        }
                }
                        /* Print deltalist */
//                        if(time_list->head != NULL)
//                        {
//                                print_list(time_list);
//                        }
                FD_ZERO(&fd_read_set);
                FD_SET(sock_timer_recv, &fd_read_set);

        }
        close(sock_timer_recv);
        close(sock_timer_send);
        return 0;

}
