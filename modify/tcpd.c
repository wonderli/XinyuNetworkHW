#include "tcpd.h"
int SEND(int socket, const void *buffer, size_t len, int flags) {
	struct sockaddr_in srv_addr;
//	struct hostent *hp;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(TCPD_PORT);
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        int ret;
	ret = sendto(socket, buffer, len, flags, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr_in));
        return ret;
}
int RECV(int socket, void *buffer, size_t len, int flags) {
	struct sockaddr_in srv_addr;
//	struct hostent *hp;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(TCPD_PORT);
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        printf("\nIN TCPD RECV\n");
	int srv_addr_length = sizeof(struct sockaddr_in);
        int ret;
	if((ret = recvfrom(socket, buffer, len, flags, (struct sockaddr *)&srv_addr, &srv_addr_length))< 0){
                perror("recv error");
        }
        return ret;

}

int RECV_CONTROL(int socket, void *buffer, size_t len, int flags) {
	struct sockaddr_in srv_addr;
//	struct hostent *hp;
	srv_addr.sin_family = AF_INET;
	//srv_addr.sin_port = htons(CONTROL_PORT_M2);
	srv_addr.sin_port = htons(CONTROL_PORT);
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int srv_addr_length = sizeof(struct sockaddr_in);
        int ret;
	if((ret = recvfrom(socket, buffer, len, flags, (struct sockaddr *)&srv_addr, &srv_addr_length))< 0){
                perror("recv error");
        }
        return ret;

}


float RTO(float M, int packet_ctrl)
{
	float A = 0;
	float D = 3000;
	float Err;
	float g = 0.125;
	float h = 0.25;
	int RTO = 0;
	if(packet_ctrl == 0)
	{
		RTO = 6000;
		return RTO;
	}
	else if(packet_ctrl == 1)
	{
		A = 50;
		if(M == 0)
		{
			RTO = 6000;
			return RTO;
		}
		else 
		{
			RTO = M;
			return RTO;
		}
	}
	else 
	{
		Err = M - A;
		A = A + g * Err;
		D = D + h * (abs(Err)-D); 
		RTO = A + 4 * D;
		return RTO;
	}
}
float RTT(struct timeval *start, struct timeval *end)
{
	float RTT =  end->tv_sec * 1e6 + end->tv_usec - start->tv_sec * 1e6 - start->tv_usec;
	return RTT;
}

