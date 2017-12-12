@echo off
setlocal

set SRCDIR=..\..\src
set INCDIR=..\..\..\..\ccwrap\vc
set CCOPTS=-EHsc

if exist smp.exe del smp.exe
if exist smp_c.exe del smp_c.exe
if exist smp_stl.exe del smp_stl.exe


cl -EHsc -DCOMPILING_FOR_ERROR -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp.cpp
cl -EHsc -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp.cpp
cl -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp_c.c
cl -EHsc -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp_stl.cpp


if exist smp.exe smp.exe
if exist smp_c.exe smp_c.exe
if exist smp_stl.exe smp_stl.exe

endlocal
