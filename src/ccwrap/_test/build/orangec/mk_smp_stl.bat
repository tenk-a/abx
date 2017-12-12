@echo off
setlocal
set SRCDIR=..\..\src
rem set INCDIR="..\..\..\..\ccwrap\orangec;C:\Program Files (x86)\Orange C 386\include"
set INCDIR="C:\Program Files (x86)\Orange C 386\include"
set COPTS=/D__CCWRAP_NO_HEADER_CFENV=1 /D__CCWRAP_NO_HEADER_ATOMIC=1 /D__CCWRAP_NO_HEADER_THREAD=1 /D__CCWRAP_NO_HEADER_MUTEX=1 /D__CCWRAP_NO_HEADER_SHARED_MUTEX=1 /D__CCWRAP_NO_HEADER_CONDITION_VARIABLE=1 /D__CCWRAP_NO_HEADER_FUTURE=1 /D__NO_WCTYPE /DNO_STEADY_CLOCK

rem if exist smp.exe del smp.exe
rem if exist smp_c.exe del smp_c.exe
if exist smp_stl.exe del smp_stl.exe

rem occ /1 /C+F /C+u /I%INCDIR% %SRCDIR%\smp.cpp
rem occ /1 /C+F /C+u /I%INCDIR% %SRCDIR%\smp_c.c
occ /1 /C+F /C+u %COPTS% /I%INCDIR% %SRCDIR%\smp_stl.cpp
rem occ /1 /C+F /C+u /DCOMPILING_FOR_ERROR /I%INCDIR% %SRCDIR%\smp.cpp

rem if exist smp.exe smp.exe
rem if exist smp_c.exe smp_c.exe
if exist smp_stl.exe smp_stl.exe

endlocal
