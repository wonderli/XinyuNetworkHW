# Makefile for client and server
all: tcpd_m1 tcpd_m2
tcpd_m1: tcpd_m1.c tcpd.h tcpd.c
	gcc -g -o tcpd_m1 tcpd_m1.c tcpd.h tcpd.c
tcpd_m2: tcpd_m2.c tcpd.h tcpd.c
	gcc -g -o tcpd_m2 tcpd_m2.c tcpd.h tcpd.c
clean:
	rm *.o tcpd_m1 tcpd_m2
