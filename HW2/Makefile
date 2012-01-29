# Makefile for client and server
all: tcpd_m1 tcpd_m2 ftpc ftps
tcpd_m1: tcpd_m1.c tcpd.h tcpd.c
	gcc -g -o tcpd_m1 tcpd_m1.c tcpd.h tcpd.c
tcpd_m2: tcpd_m2.c tcpd.h tcpd.c
	gcc -g -o tcpd_m2 tcpd_m2.c tcpd.h tcpd.c
ftpc: ftpc.c tcpd.h tcpd.c
	gcc -g -o ftpc ftpc.c tcpd.h tcpd.c
ftps: ftps.c tcpd.h tcpd.c
	gcc -g -o ftps ftps.c tcpd.h tcpd.c
clean:
	rm  tcpd_m1 tcpd_m2 ftpc ftps *~
