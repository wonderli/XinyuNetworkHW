#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#define MAX_MES_LEN 1024

main(void) /* server program called with no argument */
{
    int sock, namelen, buflen;
    char srv_buf[MAX_MES_LEN];
    struct sockaddr_in name;

    /*create socket*/
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0) {
	perror("opening datagram socket");
	exit(1);
    }

    /* create name with parameters and bind name to socket */
    name.sin_family = AF_INET;
    name.sin_port = 0;
    name.sin_addr.s_addr = INADDR_ANY;
    if(bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
	perror("getting socket name");
	exit(2);
    }
    namelen=sizeof(struct sockaddr_in);
    /* Find assigned port value and print it for client to use */
    if(getsockname(sock, (struct sockaddr *)&name, &namelen) < 0){
	perror("getting sock name");
	exit(3);
    }
    printf("Server waiting on port # %d\n", ntohs(name.sin_port));

    /* waiting for connection from client on name and print what client sends */
    namelen = sizeof(name);
    buflen = MAX_MES_LEN;
    while(1)
    {
	    if(recvfrom(sock, srv_buf, buflen, 0, (struct sockaddr *)&name, &namelen) < 0) {
		    perror("error receiving"); 
		    exit(4);
	    }
	    printf("Server receives: %s\n", srv_buf);
	    if(atoi(srv_buf) == 1)
		    break;

	    /* server sends something back using the same socket */
	    bzero(srv_buf, MAX_MES_LEN);
	    strcpy(srv_buf, "Hello back in UDP from server");
	    printf("Server sends:    %s\n", srv_buf);
	    if(sendto(sock, srv_buf, buflen, 0, (struct sockaddr *)&name, namelen) < 0) {
		    perror("sending datagram");
		    exit(5);
	    }
    }
    bzero(srv_buf, MAX_MES_LEN);
    strcpy(srv_buf, "This send from server");
    printf("Server sends this following msg:  %s\n", srv_buf);
    if(sendto(sock, srv_buf, buflen, 0, (struct sockaddr *)&name, namelen) < 0) {
	    perror("sending datagram");
	    exit(5);
    }
    /* server terminates connection, closes socket, and exits */
    close(sock);
    exit(0);
}
