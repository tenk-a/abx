@echo off
setlocal
set SRCDIR=..\..\src
rem set INCDIR="..\..\..\..\ccwrap\orangec;C:\Program Files (x86)\Orange C 386\include"
set INCDIR="C:\Program Files (x86)\Orange C 386\include"
set COPTS=/D__CCWRAP_NO_HEADER_CFENV=1

if exist smp.exe del smp.exe
if exist smp_c.exe del smp_c.exe
if exist smp_stl.exe del smp_stl.exe

occ /1 /C+F /C+u %COPTS% /DCOMPILING_FOR_ERROR /I%INCDIR% %SRCDIR%\smp.cpp
occ /1 /C+F /C+u %COPTS% /I%INCDIR% %SRCDIR%\smp.cpp
occ /1 /C+F /C+u %COPTS% /I%INCDIR% %SRCDIR%\smp_c.c
rem occ /1 /C+F /C+u %COPTS% /I%INCDIR% %SRCDIR%\smp_stl.cpp

if exist smp.exe smp.exe
if exist smp_c.exe smp_c.exe
rem if exist smp_stl.exe smp_stl.exe

endlocal
