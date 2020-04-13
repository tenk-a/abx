@echo off
setlocal

set SRCDIR=..\..\src
set CCWRAPDIR=%SRCDIR%\ccwrap
set FKSDIR=%SRCDIR%\fks
set SRCS=%SRCDIR%\abx.cpp %SRCDIR%\AbxConvFmt.cpp %SRCDIR%\AbxFiles.cpp %SRCDIR%\AbxMsgStr.cpp %SRCDIR%\AbxMsgStrJp_dbc.cpp
rem set FKSSRCS=%FKSDIR%\fks_path.c %FKSDIR%\fks_io.c %FKSDIR%\fks_io_mbs.cpp %FKSDIR%\fks_misc.c %FKSDIR%\fks_dirent.c %FKSDIR%\fks_time.c
set FKSSRCS=%FKSDIR%\fks_all.cpp
set FKSOPTS=-DFKS_SRC_DBC -DFKS_UNUSE_WIN32_PATHMATCHSPEC  -I%CCWRAPDIR%\mingw

set RelDbg=%1

set OPTS=-O2 -DNDEBUG
if /I "%RelDbg%"=="debug" set OPTS=-O0 -D_DEBUG
set OPTS=%OPTS% -D_CONSOLE -I%SRCDIR% -fpermissive -std=gnu++11

g++ -oabx.exe %OPTS% %FKSOPTS% %SRCS% %FKSSRCS% %AddLib%

if exist *.o del *.o

endlocal
