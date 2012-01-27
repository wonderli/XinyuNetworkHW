#include "tcpd.h"
int SEND(int socket, const void *buffer, size_t len, int flags) {
	struct sockaddr_in srv_addr;
	struct hostent *hp;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(TCPD_PORT);
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        printf("\nsend\n");
	return sendto(socket, buffer, len, flags, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr_in));
}
int RECV(int socket, void *buffer, size_t len, int flags) {
	struct sockaddr_in srv_addr;
	struct hostent *hp;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(TCPD_PORT);
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int srv_addr_length = sizeof(struct sockaddr_in);
	return recvfrom(socket, buffer, len, flags, (struct sockaddr *)&srv_addr, &srv_addr_length);

}
