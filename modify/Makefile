# Makefile for client and server
LIBS = -lsocket -lnsl
all: timer tcpd_m1 tcpd_m2 ftpc ftps
timer: deltalist.h deltalist.c timer.c
	gcc -g -o timer deltalist.h deltalist.c timer.c $(LIBS)
tcpd_m1: tcpd_m1.c tcpd.h tcpd.c deltalist.h deltalist.c crc.h
	gcc -g -o tcpd_m1 tcpd_m1.c tcpd.h tcpd.c deltalist.h deltalist.c crc.h $(LIBS)
tcpd_m2: tcpd_m2.c tcpd.h tcpd.c deltalist.h deltalist.c crc.h
	gcc -g -o tcpd_m2 tcpd_m2.c tcpd.h tcpd.c deltalist.h deltalist.c crc.h $(LIBS)
ftpc: ftpc.c tcpd.h tcpd.c deltalist.h deltalist.c crc.h
	gcc -g -o ftpc ftpc.c tcpd.h tcpd.c deltalist.h deltalist.c crc.h $(LIBS)
ftps: ftps.c tcpd.h tcpd.c deltalist.h deltalist.c crc.h
	gcc -g -o ftps ftps.c tcpd.h tcpd.c deltalist.h deltalist.c crc.h$(LIBS)

clean:
	rm  tcpd_m1 tcpd_m2 ftpc ftps recv/* *~
