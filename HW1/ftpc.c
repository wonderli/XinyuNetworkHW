/* client.c using TCP */

/* Client for connecting to Internet stream server waiting on port 1040 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#define port "1040"   /* socket file name */
int file_send(int sck, char *file);
/* client program called with host name where server is run */
int main(int argc, char *argv[])
{
	int sock;                     /* initial socket descriptor */
	int  rval;                    /* returned value from a read */
	struct sockaddr_in sin_addr; /* structure for socket name 
				      * setup */
	char buf[1024] = "Hello in TCP from client";     /* message to set to server */
	struct hostent *hp;


	if(argc < 2) {
		printf("Usage: ftpc <remote-IP> <remote-port> <loca-file-transfer>\n");
		exit(1);
	}


	/* initialize socket connection in unix domain */
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("error openting datagram socket");
		exit(1);
	}

	hp = gethostbyname(argv[1]);
	if(hp == 0) {
		fprintf(stderr, "%s: unknown host\n", argv[1]);
		exit(2);
	}

	/* construct name of socket to send to */
	bcopy((void *)hp->h_addr, (void *)&sin_addr.sin_addr, hp->h_length);
	sin_addr.sin_family = AF_INET;
	sin_addr.sin_port = htons(atoi(port)); /* fixed by adding htons() */

	/* establish connection with server */
	if(connect(sock, (struct sockaddr *)&sin_addr, sizeof(struct sockaddr_in)) < 0) {
		close(sock);
		perror("error connecting stream socket");
		exit(1);
	}

	/* write buf to sock */
	if(write(sock, buf, 1024) < 0) {
		perror("error writing on stream socket");
		exit(1);
	}
	printf("Client sends:    %s\n", buf);

	if(read(sock, buf, 1024) < 0) {
		perror("error reading on stream socket");
		exit(1);
	}
	printf("Client receives: %s\n", buf);
	return 0;

}
int file_send (int sck, char *file)
{
  int nread;
  int send_file;
  if (send_file = open (file, O_RDONLY) < 0)
    {
      perror ("File open error");
      return 1;
    }
  bzero (rbuf1, MAXBUF);
  for (;;)
    {
      if ((nread = read (send_file, rbuf1, MAXBUF)) < MAXBUF)
	{
	  send (sck, rbuf1, nread, 0);
	  break;
	}
      else
	send (sck, rbuf1, MAXBUF, 0);
    }
  close (send_file);
  if (close (sck) < 0)
    {
      printf ("close error\n");
      return 1;
    }
  return 0;
}
