/*client part of TCPD*/
/*connection port is 3110*/
#include "tcpd.h"
#include "troll.h"
int main() /* server program called with no argument */
{
    int sock, namelen, buflen;
    char srv_buf[MAXBUF];
    int sock_to_troll, sock_to_troll_len;
    struct sockaddr_in name;
    struct sockaddr_in troll_addr;
    /*create from ftpc socket*/
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0) {
	perror("opening datagram socket for recv from ftpc");
	exit(1);
    }
    /*create troll socket*/
    sock_to_troll = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_to_troll < 0) {
	    perror("opening datagram socket for send to troll");
	    exit(1);
    }

    /* create name with parameters and bind name to socket */
    name.sin_family = AF_INET;
    name.sin_port = htons(TCPD_PORT);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
	perror("Recv socket Bind failed");
	exit(2);
    }
    /* create troll_addr with parameters and bind troll_addr to socket */
    troll_addr.sin_family = AF_INET;
    troll_addr.sin_port = htons(TROLL_PORT);
    troll_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sock_to_troll, (struct sockaddr *)&troll_addr, sizeof(troll_addr)) < 0) {
	perror("Send socket Bind failed");
	exit(2);
    }


    /* Find assigned port value and print it for client to use */
    /*
    if(getsockname(sock, (struct sockaddr *)&name, &namelen) < 0){
	perror("getting sock name");
	exit(3);
    }
    printf("Server waiting on port # %d\n", ntohs(name.sin_port));
    */
	
    /* waiting for connection from client on name and print what client sends */
    /*namelen = sizeof(name);*/
    namelen=sizeof(struct sockaddr_in);
    sock_to_troll_len=sizeof(struct sockaddr_in);
    buflen = MAXBUF;
    while(1) {
	    if((buflen = recvfrom(sock, srv_buf, MAXBUF, 0, (struct sockaddr *)&name, &namelen)) < 0){
		    perror("error receiving from ftpc"); 
		    exit(4);
	    }
	    if((sendto(sock_to_troll, srv_buf, buflen, 0, (struct sockaddr *)&troll_addr, sock_to_troll_len)) < 0){
		    perror("sending datagram to troll");
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
