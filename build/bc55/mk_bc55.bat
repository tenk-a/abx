@echo off
setlocal
rem
rem usage> mk_bc55.bat [debug]
rem

set SRCDIR=..\..\src
set CCWRAPDIR=%SRCDIR%\ccwrap
set FKSDIR=%SRCDIR%\fks
set SRCS=%SRCDIR%\abx.cpp %SRCDIR%\abx_usage.cpp %SRCDIR%\AbxConvFmt.cpp %SRCDIR%\AbxFiles.cpp %SRCDIR%\subr.cpp %FKSDIR%\fks_path.c %FKSDIR%\fks_io.c %FKSDIR%\fks_misc.c %FKSDIR%\fks_dirent.c %FKSDIR%\fks_time.c

set RelDbg=%1

set OPTS=-Ox -DNDEBUG
if /I "%RelDbg%"=="debug" set OPTS=-O0 -D_DEBUG
set OPTS=%OPTS% -w-8026 -w-8027 -D_CONSOLE -I%SRCDIR% -I%FKSDIR% -I%CCWRAPDIR%\borland

bcc32 %OPTS% %SRCS% %AddLib%

endlocal
