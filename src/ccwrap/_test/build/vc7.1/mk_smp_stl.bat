@echo off
setlocal

set SRCDIR=..\..\src
set INCDIR=..\..\..\..\ccwrap\vc
set INCDIR2=..\..\..\..\ccwrap\boost2std
set BOOSTDIR=d:\libs_vc\boost_1_65_1
set BOOSTLIBDIR=%BOOSTDIR%\stage\vc71_Win32\lib\

if exist smp_stl.exe del smp_stl.exe

cl -EHsc -MT -I%INCDIR% -I%SRCDIR% -I%INCDIR2% -I%BOOSTDIR% %SRCDIR%\smp_stl.cpp -link /LIBPATH:%BOOSTLIBDIR%

if exist smp_stl.exe smp_stl.exe

endlocal
