@echo off

set executable=%1
set input=%executable%_test.in
set output=%executable%_test.res

type %input% | %executable% > %output%