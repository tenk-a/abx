@echo off
setlocal
set SRCDIR=..\..\src
set INCDIR=..\..\..\..\ccwrap\borland

if exist smp.exe del smp.exe
if exist smp_c.exe del smp_c.exe

bcc32c -DCOMPILING_FOR_ERROR -I%SRCDIR% -I%INCDIR% -I%INCDIR2% -I%BOOSTDIR% %SRCDIR%\smp.cpp
bcc32c -I%SRCDIR% -I%INCDIR% -I%INCDIR2% -I%BOOSTDIR% %SRCDIR%\smp.cpp
bcc32c -I%SRCDIR% -I%INCDIR% -I%INCDIR2% -I%BOOSTDIR% %SRCDIR%\smp_c.c

if exist smp.exe smp.exe
if exist smp_c.exe smp_c.exe

endlocal
