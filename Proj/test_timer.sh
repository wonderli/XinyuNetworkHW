#!/bin/sh
#Test timer
#gcc -g deltalist.h deltalist.c  -o delta
rm timer timer_test
gcc -g deltalist.h deltalist.c timer.c -o timer
gcc -g deltalist.h deltalist.c timer_test.c -o timer_test
