@echo off
set cc = gcc
set cflags = -g -m64 -Wall -pedantic-errors
del csrc.txt

for /r %%f in (*.c) do (
    echo %%f >> csrc.txt
)

FOR /F "Usebackq Tokens=* delims=" %%# IN ("csrc.txt") DO (
  echo|set /p=%%#^^^ 
) >> temp.file
move /y "temp.file" "csrc.txt" 

set /p csrc=<csrc.txt

@echo on
gcc -g -m64 -Wall -pedantic-errors %csrc% -o kod.exe

@echo off
del csrc.txt
@echo on