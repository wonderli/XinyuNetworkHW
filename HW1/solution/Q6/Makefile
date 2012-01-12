# Makefile for client and server

CC = gcc
OBJCLI = client.c 
OBJSRV = server.c
CFLAGS = #-ansi -lxnet
# setup for system
LIBS =

all: cli srv 

cli:	$(OBJCLI)
	$(CC) $(CFLAGS) -o $@ $(OBJCLI) $(LIBS)

srv:	$(OBJSRV)
	$(CC) $(CFLAGS) -o $@ $(OBJSRV) $(LIBS)

clean:
	rm cli srv
