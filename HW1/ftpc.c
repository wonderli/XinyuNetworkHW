/* client.c using TCP */

/* Client for connecting to Internet stream server waiting on port 1040 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#define port "1050"   /* socket file name */
#define MAXBUF 1000
int file_send(int sck, char *file);
/* client program called with host name where server is run */
int main(int argc, char *argv[])
{
	int sock;                     /* initial socket descriptor */
	int  rval;                    /* returned value from a read */
	struct sockaddr_in sin_addr; /* structure for socket name 
				      * setup */
	struct hostent *hp;
	char filename[MAXBUF];
	if(argc < 2) {
		printf("Usage: ftpc <remote-IP> <remote-port> <loca-file-transfer>\n");
		exit(1);
	}
	strcpy(filename, argv[2]);
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
	/* 
	int file_size = get_file_size(filename);
	printf("filesize is %d\n", file_size);
	printf("filename is %s\n", filename);
	*/
	file_send(sock,filename);  

	/* write buf to sock */
	/*
	if(write(sock, buf, MAXBUF) < 0) {
		perror("error writing on stream socket");
		exit(1);
	}
	printf("Client sends:    %s\n", buf);
	*/
	char *buf;
	buf = (char*)malloc(MAXBUF);
	if(read(sock, buf, MAXBUF) < 0) {
		perror("error reading on stream socket");
		exit(1);
	}
	if (close (sock) < 0)
	{
		printf ("close socket error\n");
		return 1;
	}

	printf("Client receives: %s\n", buf);
	return 0;

}
int get_file_size(char *filename)
{
	/*
	struct stat file_stat;
	if(stat(filename, &file_stat) != -1)
	return file_stat.st_size;
	*/
	FILE *fp;
	int start = 0; //Read start point
	int file_size = 0;
	fp = fopen(filename, "rb");
	if(fp == NULL)
	{
		perror("File not found!!!");
	}
	else
	{
		start = ftell(fp);
		fseek(fp, 0L, SEEK_END);
		file_size = ftell(fp);
		fseek(fp, start,SEEK_SET);
	}
	fclose(fp);
	return file_size;
}
int file_send (int sck, char *filename)
{
  int nread;
  int send_file;
  char *read_file_buf;
  read_file_buf = (char*) malloc(MAXBUF);
  if ((send_file = open (filename, O_RDONLY) )< 0)
  {
	  perror ("File open error");
	  return 1;
  }
  bzero(read_file_buf, MAXBUF);
  int file_size = get_file_size(filename);
  printf("filesize is %d\n", file_size);
  printf("filename is %s\n", filename);
  bcopy(&file_size, read_file_buf, sizeof(int));
  bcopy(filename, read_file_buf+4, 20);
  if ((nread = read (send_file, read_file_buf+24, MAXBUF-24)) < (MAXBUF - 24))
  {
	  send (sck, read_file_buf, nread+24, 0);
  }
  else if (nread == (MAXBUF -24))
  {
	  send (sck, read_file_buf, nread+24, 0);

	  for(;;)
	  {
		  if ((nread = read (send_file, read_file_buf, MAXBUF)) < MAXBUF)
		  {
			  send (sck, read_file_buf, nread, 0);
			  //printf("The nread is %d", nread);
			  break;
		  }
		  else
			  send (sck, read_file_buf, MAXBUF, 0);
	  } 
  }
  /*
  if ((nread = read (send_file, read_file_buf+24, MAXBUF-24)) < (MAXBUF - 24))
  {
	  send (sck, read_file_buf, nread+24, 0);
  }
  else
	  send (sck, read_file_buf, MAXBUF, 0);
  */
  close (send_file);
  return 0;
}
