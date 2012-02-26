# Makefile for client and server
LIBS = -lsocket -lnsl
all: tcpd_m1 tcpd_m2 ftpc ftps
tcpd_m1: tcpd_m1.c tcpd.h tcpd.c
	gcc -g -o tcpd_m1 tcpd_m1.c tcpd.h tcpd.c $(LIBS)
tcpd_m2: tcpd_m2.c tcpd.h tcpd.c
	gcc -g -o tcpd_m2 tcpd_m2.c tcpd.h tcpd.c $(LIBS)
ftpc: ftpc.c tcpd.h tcpd.c
	gcc -g -o ftpc ftpc.c tcpd.h tcpd.c $(LIBS)
ftps: ftps.c tcpd.h tcpd.c
	gcc -g -o ftps ftps.c tcpd.h tcpd.c $(LIBS)

clean:
	rm  tcpd_m1 tcpd_m2 ftpc ftps recv/* *~
