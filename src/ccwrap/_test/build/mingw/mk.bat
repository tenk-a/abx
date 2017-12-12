@echo off
setlocal
set SRCDIR=..\..\src
set INCDIR=..\..\..\..\ccwrap\mingw
set COPTS=

if exist smp.exe del smp.exe
if exist smp_c.exe del smp_c.exe
if exist smp_stl.exe del smp_stl.exe

rem g++ -v -o smp.exe -std=c++11 -Wall -UNO_OLDNAMES -U__STRICT_ANSI__ -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp.cpp
g++ -o smp.exe -std=c++11 -Wall %COPTS% -I%INCDIR% -I%SRCDIR% -DCOMPILING_FOR_ERROR %SRCDIR%\smp.cpp
g++ -o smp.exe -std=c++11 -Wall %COPTS% -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp.cpp
gcc -o smp_c.exe -std=gnu11 -Wall %COPTS% -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp_c.c
g++ -o smp_stl.exe -std=c++1y %COPTS% -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp_stl.cpp


if exist smp.exe smp.exe
if exist smp_c.exe smp_c.exe
if exist smp_stl.exe smp_stl.exe

endlocal
