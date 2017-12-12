@echo off
setlocal
set SRCDIR=..\..\src
set INCDIR=..\..\..\..\ccwrap\dmc

if exist smp.exe del smp.exe
if exist smp_c.exe del smp_c.exe

dmc -DCOMPILING_FOR_ERROR -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp.cpp kernel32.lib gdi32.lib
dmc -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp.cpp kernel32.lib gdi32.lib
dmc -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp_c.c kernel32.lib gdi32.lib

if exist smp.exe   smp.exe
if exist smp_c.exe smp_c.exe

endlocal
