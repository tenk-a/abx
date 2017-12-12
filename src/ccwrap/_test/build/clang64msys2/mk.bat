@echo off
setlocal
set SRCDIR=..\..\src
rem set INCDIR=..\..\..\..\ccwrap\mingw
set INCDIR=
set COPTS=

if exist smp.exe del smp.exe
if exist smp_c.exe del smp_c.exe
if exist smp_stl.exe del smp_stl.exe

clang++ -o smp.exe -std=c++11 -Wall %COPTS% -I%SRCDIR% -DCOMPILING_FOR_ERROR %SRCDIR%\smp.cpp
clang++ -o smp.exe -std=c++11 -Wall %COPTS% -I%SRCDIR% %SRCDIR%\smp.cpp
clang   -o smp_c.exe -std=c11 -Wall %COPTS% -I%SRCDIR% %SRCDIR%\smp_c.c
clang++ -o smp_stl.exe -std=c++14 %COPTS% -I%SRCDIR% %SRCDIR%\smp_stl.cpp


if exist smp.exe smp.exe
if exist smp_c.exe smp_c.exe
if exist smp_stl.exe smp_stl.exe

endlocal
