@echo off
setlocal

set SRCDIR=..\..\src
set INCDIR=..\..\..\..\ccwrap\vc
set INCDIR2=..\..\..\..\ccwrap\boost2std
set BOOSTDIR=d:\libs_vc\boost_1_65_1
set BOOSTLIBDIR=%BOOSTDIR%\stage\vc120_Win32\lib
set CCOPTS=-EHsc

if exist smp.exe del smp.exe
if exist smp_c.exe del smp_c.exe
if exist smp_stl.exe del smp_stl.exe

cl -EHsc -DCOMPILING_FOR_ERROR -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp.cpp
cl -EHsc -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp.cpp
cl -I%INCDIR% -I%SRCDIR% %SRCDIR%\smp_c.c
cl -EHsc -I%INCDIR% -I%SRCDIR% -I%INCDIR2% -I%BOOSTDIR% %SRCDIR%\smp_stl.cpp -link /LIBPATH:%BOOSTLIBDIR%

if exist smp.exe smp.exe
if exist smp_c.exe smp_c.exe
if exist smp_stl.exe smp_stl.exe

endlocal
