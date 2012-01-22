#include "tcpd.h"
int SEND(int socket, const void *buffer, size_t len, int flags) {
	struct sockaddr_in srv_addr;
	struct hostent *hp;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(TCPD_PORT);
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	return sendto(socket, buffer, len, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr_in));
}
int RECV(int socket, void *buffer, size_t length, int flags) {
	struct sockaddr_in srv_addr;
	struct hostent *hp;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(TCPD_PORT);
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	return recvfrom(socket, buffer, len, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr_in));

}
