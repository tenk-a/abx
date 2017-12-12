@echo off
setlocal
set SRCDIR=..\..\src
set INCDIR=..\..\..\..\ccwrap\ow

if exist smp.exe   del smp.exe
if exist smp_c.exe del smp_c.exe

wcl386.exe -cc++ -xs -xr -bcl=nt -DCOMPILING_FOR_ERROR -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp.cpp
wcl386.exe -cc++ -xs -xr -bcl=nt -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp.cpp
wcl386.exe -cc           -bcl=nt -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp_c.c

if exist smp.exe   smp.exe
if exist smp_c.exe smp_c.exe

endlocal
