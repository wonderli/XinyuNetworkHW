# Makefile for client and server
CC = gcc
OBJCLI = ftpc.c 
OBJSRV = ftps.c
CFLAGS = -ansi #-lxnet
# setup for system
LIBS =
all: ftpc ftps
ftpc:	$(OBJCLI)
	$(CC) $(CFLAGS) -o $@ $(OBJCLI) $(LIBS) -g
ftps:	$(OBJSRV)
	$(CC) $(CFLAGS) -o $@ $(OBJSRV) $(LIBS) -g
clean:
	rm ftpc ftps *~

