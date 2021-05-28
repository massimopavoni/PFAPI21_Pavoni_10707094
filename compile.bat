@echo off

set executable=%1
set source=%executable%.c

gcc -DEVAL -std=gnu11 -O2 -pipe -static -s -o %executable% %source%  -lm