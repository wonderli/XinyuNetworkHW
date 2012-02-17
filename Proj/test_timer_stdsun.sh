#!/bin/sh
#Test timer
#gcc -g deltalist.h deltalist.c  -o delta
rm timer timer_test
gcc -g deltalist.h deltalist.c timer.c -lsocket -lnsl -o timer 
gcc -g deltalist.h deltalist.c timer_test.c -lsocket -lnsl -o timer_test 
