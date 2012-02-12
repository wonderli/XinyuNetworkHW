#!/bin/sh
#Test timer
#gcc -g deltalist.h deltalist.c  -o delta
gcc -g deltalist.h deltalist.c timer.c -o timer
gcc -g deltalist.h deltalist.c timer_test.c -o timer_test
