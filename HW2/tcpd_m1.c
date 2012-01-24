/*server part of TCPD*/
/*connection port is 1110*/
#include "tcpd.h"
int main() /* server program called with no argument */
{
    int sock, namelen, buflen;
    char srv_buf[MAXBUF];
    int sock_from_troll, sock_from_troll_len;
    struct sockaddr_in name;
    struct sockaddr_in troll_addr;
   
    /*create from ftpc socket*/
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0) {
	perror("opening datagram socket");
	exit(1);
    }
    /*create troll socket*/
    sock_from_troll = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_from_troll < 0) {
	    perror("opening datagram socket for send to troll");
	    exit(1);
    }
    /* create name with parameters and bind name to socket */
    name.sin_family = AF_INET;
    name.sin_port = htons(TCPD_PORT);
    name.sin_addr.s_addr = INADDR_ANY;
    if(bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
	perror("getting socket name");
	exit(2);
    }
    /* create troll_addr with parameters and bind troll_addr to socket */
    troll_addr.sin_family = AF_INET;
    troll_addr.sin_port = htons(TROLL_PORT);
    troll_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sock_from_troll, (struct sockaddr *)&troll_addr, sizeof(troll_addr)) < 0) {
	perror("Send socket Bind failed");
	exit(2);
    }


    /* Find assigned port value and print it for client to use */
    /*if(getsockname(sock, (struct sockaddr *)&name, &namelen) < 0){
	perror("getting sock name");
	exit(3);
    }
    printf("Server waiting on port # %d\n", ntohs(name.sin_port));
    */
    namelen=sizeof(struct sockaddr_in);
    sock_from_troll_len=sizeof(struct sockaddr_in);

    /* waiting for connection from client on name and print what client sends */
    /*namelen = sizeof(name);*/
    buflen = MAXBUF;
    while(1) {
	    if((buflen = recvfrom(sock_from_troll, srv_buf, MAXBUF, 0, (struct sockaddr *)&troll_addr, &sock_from_troll_len)) < 0){
		    perror("error receiving"); 
		    exit(4);
	    }
	    if((sendto(sock, srv_buf, buflen, 0, (struct sockaddr *)&name, namelen)) < 0){
		    perror("sending datagram");
		    exit(5);
	    }
	    bzero(srv_buf, buflen);
	    printf("Server receives: %s\n", srv_buf);
    }
    /* server sends something back using the same socket */
    /*
    bzero(srv_buf, MAXBUF);
    strcpy(srv_buf, "Hello back in UDP from server");
    printf("Server sends:    %s\n", srv_buf);
    if(sendto(sock, srv_buf, buflen, 0, (struct sockaddr *)&name, namelen) < 0) {
	perror("sending datagram");
	exit(5);
    }
    */

    /* server terminates connection, closes socket, and exits */
    close(sock);
    exit(0);
}

