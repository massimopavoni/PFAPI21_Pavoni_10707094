@echo off

set executable=%1
set source=%executable%.c

@echo on

gcc -DEVAL -std=gnu11 -O2 -pipe -static -s -o %executable% %source%  -lm -Wall