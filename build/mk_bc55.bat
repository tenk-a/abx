@echo off
setlocal
rem
rem usage> mk_bc55.bat [debug]
rem

set SRCDIR=..\src
set SRCS=%SRCDIR%\abx.cpp %SRCDIR%\abx_usage.cpp %SRCDIR%\subr.cpp %SRCDIR%\fks\fks_path.c %SRCDIR%\fks\fks_io.c %SRCDIR%\fks\fks_misc.c %SRCDIR%\fks\fks_dirent.c %SRCDIR%\fks\fks_time.c

set RelDbg=%1

set OPTS=%OPTS% -Ox -DNDEBUG
if /I "%RelDbg%"=="debug" set OPTS=-O0 -D_DEBUG
set OPTS=%OPTS% -w-8026 -w-8027 -D_CONSOLE -I%SRCDIR% -I%SRCDIR%\ccwrap\borland %EnableOptXM%

bcc32 %OPTS% %SRCS% %AddLib%

goto :END

:END
endlocal
