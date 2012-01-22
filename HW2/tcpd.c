#include "tcpd.h"
int SEND(int socket, const void *buffer, size_t len, int flags) {
	struct sockaddr_in srv_addr;
	struct hostent *hp;
	int ret = 0;
	hp = gethostbyname("localhost");
	if(hp == 0) {
		perror("gethostbyname(\"localhost\") error");
		exit(1);
	}
	bcopy((void *)hp->h_addr, (void *)$srv_addr.sin_addr, hp->h_length);
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(TCPD_PORT);
	return sendto(socket, buffer, len, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr_in));
}
int RECV(int socket, void *buffer, size_t length, int flags) {

}
