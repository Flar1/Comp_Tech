#!/bin/sh

bison -o incalc.c incalc.y
gcc -Wall -o incalc incalc.c -lm
rm incalc.c

