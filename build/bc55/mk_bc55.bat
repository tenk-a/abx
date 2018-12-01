@echo off
setlocal
rem
rem usage> mk_bc55.bat [debug]
rem

set SRCDIR=..\..\src
set CCWRAPDIR=%SRCDIR%\ccwrap
set FKSDIR=%SRCDIR%\fks
set SRCS=%SRCDIR%\abx.cpp %SRCDIR%\AbxConvFmt.cpp %SRCDIR%\AbxFiles.cpp %SRCDIR%\AbxMsgStr.cpp %SRCDIR%\AbxMsgStrJp_dbc.cpp
set FKSSRCS=%FKSDIR%\fks_path.c %FKSDIR%\fks_io.c %FKSDIR%\fks_io_mbs.cpp %FKSDIR%\fks_misc.c %FKSDIR%\fks_dirent.c %FKSDIR%\fks_time.c
set FKSOPTS=-DFKS_SRC_DBC -DFKS_UNUSE_WIN32_PATHMATCHSPEC  -I%FKSDIR% -I%CCWRAPDIR%\borland

set RelDbg=%1

set OPTS=-Ox -DNDEBUG
if /I "%RelDbg%"=="debug" set OPTS=-O0 -D_DEBUG
set OPTS=%OPTS% -w-8026 -w-8027 -D_CONSOLE -I%SRCDIR%
rem -DABX_USE_JAPAN

bcc32 %OPTS% %FKSOPTS% %SRCS% %FKSSRCS% %AddLib%

endlocal
