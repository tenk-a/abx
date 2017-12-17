@echo off
@setlocal
rem
rem usage> mk_vc.bat [debug]
rem

set RelDbg=%1

rem Empty if you do not enable the -xm option
set EnableOptXM=-DENABLE_MT_X
rem When enabling the -xm option, boost library is required for vc 8/9/10
rem Change to your boost directory.
set BOOST_DIR=\libs_vc\boost_1_65_1
rem Set up a directory with boost's .lib
set BOOST_LIB_DIR=


rem Check VC's toolset and architecture
call :check_toolset_arch


if "%ToolSet%"=="vc71"  set EnableOptXM=

set UseBoost=
if "%ToolSet%"=="vc80"  set UseBoost=1
if "%ToolSet%"=="vc90"  set UseBoost=1
if "%ToolSet%"=="vc100" set UseBoost=1

if "%UseBoost%"=="1"    set UseCcWrap=1
if "%ToolSet%"=="vc71"  set UseCcWrap=1
if "%ToolSet%"=="vc110" set UseCcWrap=1
if "%ToolSet%"=="vc120" set UseCcWrap=1

rem echo %ToolSet% %Arch% B=%UseBoost% L=%UseCcWrap%

set SRCDIR=..\src

set OPTS=-Ox -DNDEBUG -MT
if /I "%RelDbg%"=="debug" set OPTS=-O0 -D_DEBUG -MTd
set OPTS=%OPTS% -W4 -wd4996 -EHac %EnableOptXM%
if "%UseCcWrap%"=="1" set OPTS=%OPTS% -DUSE_CXX11LESS -I%SRCDIR%\ccwrap\vc

set AddLib=
if "%UseBoost%"=="" goto SKIP_1
  if "%BOOST_LIB_DIR%"=="" set BOOST_LIB_DIR=%BOOST_DIR%\stage\%ToolSet%_%Arch%\lib
  rem If BOOST_LIB_DIR is empty set the directory generated by bld_lib_bat
  set OPTS=%OPTS% -I%SRCDIR%\ccwrap\boost2std -I%BOOST_DIR%
  set AddLib=/link/LIBPATH:%BOOST_LIB_DIR%
:SKIP_1

set SRCS=%SRCDIR%\abx.cpp %SRCDIR%\abxmt.cpp %SRCDIR%\subr.cpp


cl %OPTS% %SRCS% %AddLib%


goto :END


rem
rem
rem
:check_toolset_arch
if /I not "%PATH:Microsoft Visual Studio\2017=%"=="%PATH%"    set ToolSet=vc141
if /I not "%PATH:Microsoft Visual Studio 14.0\VC=%"=="%PATH%" set ToolSet=vc140
if /I not "%PATH:Microsoft Visual Studio 13.0\VC=%"=="%PATH%" set ToolSet=vc130
if /I not "%PATH:Microsoft Visual Studio 12.0\VC=%"=="%PATH%" set ToolSet=vc120
if /I not "%PATH:Microsoft Visual Studio 11.0\VC=%"=="%PATH%" set ToolSet=vc110
if /I not "%PATH:Microsoft Visual Studio 10.0\VC=%"=="%PATH%" set ToolSet=vc100
if /I not "%PATH:Microsoft Visual Studio 9.0\VC=%"=="%PATH%"  set ToolSet=vc90
if /I not "%PATH:Microsoft Visual Studio 8\VC=%"=="%PATH%"    set ToolSet=vc80
if /I not "%PATH:Microsoft Visual Studio .NET 2003\VC=%"=="%PATH%" set ToolSet=vc71

if /I not "%PATH:Microsoft Visual Studio 14.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
if /I not "%PATH:Microsoft Visual Studio 13.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
if /I not "%PATH:Microsoft Visual Studio 12.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
if /I not "%PATH:Microsoft Visual Studio 11.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
if /I not "%PATH:Microsoft Visual Studio 10.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
if /I not "%PATH:Microsoft Visual Studio 9.0\VC\BIN\amd64=%"=="%PATH%"  set Arch=x64
if /I not "%PATH:Microsoft Visual Studio 8\VC\BIN\amd64=%"=="%PATH%"    set Arch=x64

if "%Arch%"=="" set Arch=Win32

exit /b


rem
:END
@endlocal
