@echo off

set executable=%1

if exist my_outputs rd /s /q my_outputs
md my_outputs

for /l %%x in (1, 1, 6) do (
  type open_tests\input_%%x | %executable% > my_outputs\output_%%x
  fc open_tests\output_%%x my_outputs\output_%%x
)