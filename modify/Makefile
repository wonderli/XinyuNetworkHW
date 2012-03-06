# Makefile for client and server
LIBS = -lsocket -lnsl
all: timer tcpd_m1 tcpd_m2 ftpc ftps
timer: port.h deltalist.h deltalist.c timer.c
	gcc -g -o timer deltalist.h port.h deltalist.c timer.c $(LIBS)
tcpd_m1: tcpd_m1.c port.h tcpd.h tcpd.c deltalist.h deltalist.c crc.h
	gcc -g -o tcpd_m1 tcpd_m1.c port.h tcpd.h tcpd.c deltalist.h deltalist.c crc.h $(LIBS)
tcpd_m2: tcpd_m2.c port.h tcpd.h tcpd.c deltalist.h deltalist.c crc.h
	gcc -g -o tcpd_m2 tcpd_m2.c port.h tcpd.h tcpd.c deltalist.h deltalist.c crc.h $(LIBS)
ftpc: ftpc.c port.h tcpd.h tcpd.c deltalist.h deltalist.c crc.h
	gcc -g -o ftpc ftpc.c port.h tcpd.h port.h tcpd.c deltalist.h deltalist.c crc.h $(LIBS)
ftps: ftps.c port.h tcpd.h tcpd.c deltalist.h deltalist.c crc.h
	gcc -g -o ftps ftps.c port.h tcpd.h tcpd.c deltalist.h deltalist.c crc.h $(LIBS)

clean:
	rm -f tcpd_m1 tcpd_m2 ftpc ftps recv/* *~
