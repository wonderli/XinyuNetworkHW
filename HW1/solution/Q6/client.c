/* Example: client.c sending and receiving datagrams using UDP */
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define MAX_MES_LEN 1024

struct sockaddr_in name;
int send_udp(int sck, char* buf);
/* client program called with host name and port number of server */
int main(int argc, char *argv[])
{
    int sock, buflen;
    //char cli_buf[MAX_MES_LEN] = "Hello in UDP from client";
    //char *send_buf;
    //send_buf = (char*)malloc(MAX_MES_LEN);
    char send_buf[MAX_MES_LEN];
    bzero(send_buf, MAX_MES_LEN);
    struct hostent *hp, *gethostbyname();

    if(argc < 3) {
	printf("usage: cli remote_host_name remote_port_number\n");
	exit(1);
    }

    /* create socket for connecting to server */
    sock = socket(AF_INET, SOCK_DGRAM,0);
    if(sock < 0) {
	perror("opening datagram socket");
	exit(2);
    }

    /* construct name for connecting to server */
    name.sin_family = AF_INET;
    name.sin_port = htons(atoi(argv[2]));

    /* convert hostname to IP address and enter into name */
    hp = gethostbyname(argv[1]);
    if(hp == 0) {
	fprintf(stderr, "%s:unknown host\n", argv[1]);
	exit(3);
    }
    bcopy((char *)hp->h_addr, (char *)&name.sin_addr, hp->h_length);
    pid_t pid;
    char recv_buf[MAX_MES_LEN];
    if((pid = fork()) < 0){
	    perror("fork error!!!");
    } else if(pid == 0) {
	    printf("In the child process, process id is %d\n", getpid());
	    sprintf(send_buf,"msg send from child, process id %d\n", getpid());
	    send_udp(sock,send_buf);
	    if(recv(sock, recv_buf, MAX_MES_LEN,0)<0) {
		    perror("error recv msg");
		    exit(5);
	    }
	    printf("In child process %d, Client receives: %s\n", getpid(),recv_buf);

    } else {
	    printf("In the parent process, process id is %d\n", getpid());
	    sprintf(send_buf,"msg send from parent, process id %d\n", getpid());
	    send_udp(sock, send_buf);
	    if(recv(sock, recv_buf, MAX_MES_LEN,0)<0) {
		    perror("error recv msg");
		    exit(5);
	    }
	    printf("In parent process %d, Client receives: %s\n", getpid(), recv_buf);

    }
    sleep(5);
    bzero(send_buf, MAX_MES_LEN);
    strcpy(send_buf,"1");
    send_udp(sock,send_buf);
    if(recv(sock, recv_buf, MAX_MES_LEN,0)<0) {
	    perror("error recv msg");
	    exit(5);
    }
    printf("In process %d, Client receives: %s\n", getpid(), recv_buf);
    /* close connection */
    close(sock);
    return 0;
}
int send_udp(int sck, char* buf)
{
	if(sendto(sck, buf, MAX_MES_LEN, 0, (struct sockaddr *)&name, sizeof(name)) <0) {
		perror("sending datagram message");
		exit(4);
	}
	return 0;

}
