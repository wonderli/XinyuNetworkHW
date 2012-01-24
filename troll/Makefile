# Makefile for Network Troll to work under Solaris 2

#CFLAGS = -I/usr/ucbinclude
CFLAGS = 
#CFLAGS = -g
#CIFLAGS = -l -sDistrib
RCSDIFFFLAGS = -c
CC = gcc 
#LIB = -lm -lsocket -lc -lnsl -L/usr/ucblib -R/usr/ucblib -lucb
LIB = -lm #-lsocket #-lnsl 
# LIB = -lm -lc

ALL = troll fromtroll totroll
SRC = Makefile fromtroll.c totroll.c troll_bugfixed.c troll.1 troll.h

all: $(ALL)

clean: true
	rm -f *.o $(ALL)

troll: troll.o
	$(CC) $(CFLAGS) -o troll troll.o $(LIB)

fromtroll: fromtroll.o
	$(CC) $(CFLAGS) -o fromtroll fromtroll.o $(LIB)

totroll: totroll.o
	$(CC) $(CFLAGS) -o totroll totroll.o $(LIB)

ci: true
	ci $(CIFLAGS) $(SRC)

rcsdiff: true
	rcsdiff $(RCSDIFFFLAGS) $(SRC)

distrib: true
#	rsh garfield -l cs640 'cd troll; rm -f $(SRC)'
	rcp $(SRC) cs640@garfield:troll

true:

