#!/bin/sh
#Test timer
#gcc -g deltalist.h deltalist.c  -o delta
rm timer timer_test
LIB=-lsocket -lnsl
gcc -g deltalist.h deltalist.c timer.c -o timer $LIB
gcc -g deltalist.h deltalist.c timer_test.c -o timer_test $LIB
