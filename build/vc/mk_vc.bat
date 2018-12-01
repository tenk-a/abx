@echo off
@setlocal
rem
rem usage> mk_vc.bat [debug]
rem

set RelDbg=%1

set USEJAPAN=1
set SRCDIR=..\..\src
set SRCS=%SRCDIR%\abx.cpp %SRCDIR%\AbxConvFmt.cpp %SRCDIR%\AbxFiles.cpp %SRCDIR%\abxmt.cpp %SRCDIR%\AbxMsgStr.cpp
set EXENAME=abx.exe

set CCWRAPDIR=%SRCDIR%\ccwrap
set FKSDIR=%SRCDIR%\fks
set FKSSRCS=%FKSDIR%\fks_path.c %FKSDIR%\fks_io.c %FKSDIR%\fks_io_mbs.c %FKSDIR%\fks_misc.c %FKSDIR%\fks_dirent.c %FKSDIR%\fks_time.c

rem Empty if you do not enable the -xm option
set EnableOptXM=-DENABLE_MT_X
rem When enabling the -xm option, boost library is required for vc 8/9/10
rem Change to your boost directory.
set BOOST_DIR=\libs_vc\boost_1_68_0
rem Set up a directory with boost's .lib
set BOOST_LIB_DIR=


rem Check VC's toolset and architecture
call :check_toolset_arch

if "%ToolSet%"=="vc71"  set EnableOptXM=

set UseBoost=
if "%ToolSet%"=="vc80"  set UseBoost=1
if "%ToolSet%"=="vc90"  set UseBoost=1
if "%ToolSet%"=="vc100" set UseBoost=1

set UseCcWrap=
if "%UseBoost%"=="1"    set UseCcWrap=1
if "%ToolSet%"=="vc71"  set UseCcWrap=1
if "%ToolSet%"=="vc110" set UseCcWrap=1
if "%ToolSet%"=="vc120" set UseCcWrap=1

rem echo %ToolSet% %Arch% B=%UseBoost% L=%UseCcWrap%
set UTF8=
if "%ToolSet%"=="vc140" set UTF8=-utf-8
if "%ToolSet%"=="vc141" set UTF8=-utf-8
set DBC=
if "%USEJAPAN%"=="" goto SKIP_JAPAN
if "%UTF8%"=="" (
  set DBC=-DFKS_SRC_DBC
  if not exist utf8todbc.exe (
    cl -TP -Ox -EHac -DFKS_SRC_DBC -DNDEBUG -I%SRCDIR% -I%FKSDIR% %SRCDIR%\utf8todbc\utf8todbc.cpp %FKSDIR%\fks_path.c %FKSDIR%\fks_io.c %FKSDIR%\fks_io_mbs.c
    del *.obj
  )
  utf8todbc.exe %SRCDIR%\AbxMsgStrJp.cpp >%SRCDIR%\AbxMsgStrJp_dbc.cpp
  set SRCS=%SRCS% %SRCDIR%\AbxMsgStrJp_dbc.cpp
) else (
  set SRCS=%SRCS% %SRCDIR%\AbxMsgStrJp.cpp
)
rem set OPTS=%OPTS% -DABX_USE_JAPAN
:SKIP_JAPAN

set OPTS=%OPTS% -TP -I%SRCDIR% -I%FKSDIR% -W4 -wd4996 -EHac %UTF8% %DBC% %EnableOptXM%
if /I "%RelDbg%"=="debug" (
  set OPTS=%OPTS% -O0 -D_DEBUG -MTd
) else (
  set OPTS=%OPTS% -Ox -DNDEBUG -MT
  set RelDbg=release
)

set OUTDIR=
if "%RelDbg%"=="" goto SKIP_1
  set OUTDIR=exe_%ToolSet%_%Arch%_%RelDbg%
  if not exist %OUTDIR% mkdir %OUTDIR%
  set OUTDIR=%OUTDIR%\
:SKIP_1

if "%UseCcWrap%"=="1" set OPTS=%OPTS% -DUSE_CXX11LESS -I%CCWRAPDIR%\vc

set AddLib=
if "%UseBoost%"=="" goto SKIP_2
  if "%BOOST_LIB_DIR%"=="" set BOOST_LIB_DIR=%BOOST_DIR%\stage\%ToolSet%_%Arch%\lib
  rem If BOOST_LIB_DIR is empty set the directory generated by bld_lib_bat
  set OPTS=%OPTS% -I%CCWRAPDIR%\boost2std -I%BOOST_DIR%
  set AddLib=/link/LIBPATH:%BOOST_LIB_DIR%
:SKIP_2


cl -Fe%OUTDIR%%EXENAME% %OPTS% %SRCS% %FKSSRCS% %AddLib%
del *.obj

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

if "%ToolSet%"=="vc141"  if /I not "%PATH:\bin\HostX64\x64=%"=="%PATH%" set Arch=x64
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
