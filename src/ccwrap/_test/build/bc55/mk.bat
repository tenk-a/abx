@echo off
setlocal
set SRCDIR=..\..\src
set INCDIR=..\..\..\..\ccwrap\borland
rem set INCDIR2=..\..\..\..\ccwrap\boost2std
rem set BOOSTDIR=d:\libs_vc\boost_1_65_0
rem bcc32 -I%SRCDIR% -I%INCDIR% %SRCDIR%\smp.cpp
rem bcc32 -I%SRCDIR% -I%INCDIR% -I%INCDIR2% %SRCDIR%\smp.cpp

if exist smp.exe del smp.exe
if exist smp_c.exe del smp_c.exe

bcc32 -I%SRCDIR% -I%INCDIR% -I%INCDIR2% -I%BOOSTDIR% %SRCDIR%\smp.cpp
bcc32 -I%SRCDIR% -I%INCDIR% -I%INCDIR2% -I%BOOSTDIR% %SRCDIR%\smp_c.c
bcc32 -DCOMPILING_FOR_ERROR -I%SRCDIR% -I%INCDIR% -I%INCDIR2% -I%BOOSTDIR% %SRCDIR%\smp.cpp

if exist smp.exe smp.exe
if exist smp_c.exe smp_c.exe

endlocal
