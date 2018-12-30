@echo off
setlocal
rem
rem usage> mk_bc55.bat [debug]
rem

set SRCDIR=..\..\src
set CCWRAPDIR=%SRCDIR%\ccwrap
set FKSDIR=%SRCDIR%\fks
set SRCS=%SRCDIR%\abx.cpp %SRCDIR%\AbxConvFmt.cpp %SRCDIR%\AbxFiles.cpp %SRCDIR%\AbxMsgStr.cpp %SRCDIR%\AbxMsgStrJp_dbc.cpp
rem set FKSSRCS=%FKSDIR%\fks_path.c %FKSDIR%\fks_io.c %FKSDIR%\fks_io_mbs.cpp %FKSDIR%\fks_mbc.c %FKSDIR%\fks_misc.c %FKSDIR%\fks_dirent.c %FKSDIR%\fks_time.c
set FKSSRCS=%FKSDIR%\fks_all.cpp
set FKSOPTS=-DFKS_SRC_DBC -DFKS_UNUSE_WIN32_PATHMATCHSPEC -I%CCWRAPDIR%\borland -I%FKSDIR%

set RelDbg=%1

set OPTS=-Ox -DNDEBUG
if /I "%RelDbg%"=="debug" set OPTS=-O0 -D_DEBUG
set OPTS=%OPTS% -w-8026 -w-8027 -D_CONSOLE -I%SRCDIR% -DFKS_OLD_CXX
rem -DABX_USE_JAPAN

bcc32 %OPTS% %FKSOPTS% %SRCS% %FKSSRCS% %AddLib%

del *.bak *.tds

endlocal
