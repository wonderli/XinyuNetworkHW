#ifndef _TIMER_H
#define _TIMER_H
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#endif
#define TCPD_PORT 3860
#define TROLL_PORT_M1 3870
#define TROLL_PORT_M2 3880
#define TIMER_PORT 3990
#define MAXBUF 1000
#define TRUE 1
#define FALSE 0
typedef struct node
{
        int time_left;
        long time;
        int seq;
        struct node *prev;
        struct node *next;
} node;

typedef struct linklist
{
        int len;
        node *head;
        node *tail;
} linklist;

struct timeval timout = {0,0};

typedef struct time_msg
{
        int seq;
        int opt;
        long time;
} time_msg;

node* creat_node(int seq, long time);
linklist* creat_list();
int insert_node(linklist *list, node *insert_node);
int cancel_node(linklist *list, int seq);
int remove_node(node *remove_node);
void print_list(linklist *list);

