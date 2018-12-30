@echo off
setlocal

set SRCDIR=..\..\src
set FKSDIR=%SRCDIR%\fks
set SRCS=%SRCDIR%\abx.cpp %SRCDIR%\AbxConvFmt.cpp %SRCDIR%\AbxFiles.cpp %SRCDIR%\AbxMsgStr.cpp %SRCDIR%\AbxMsgStrJp_dbc.cpp
rem set FKSSRCS=%FKSDIR%\fks_path.c %FKSDIR%\fks_io.c %FKSDIR%\fks_io_mbs.cpp %FKSDIR%\fks_mbc.c %FKSDIR%\fks_misc.c %FKSDIR%\fks_dirent.c %FKSDIR%\fks_time.c
set FKSSRCS=%FKSDIR%\fks_all.cpp
set FKSOPTS=-DFKS_SRC_DBC -DFKS_UNUSE_WIN32_PATHMATCHSPEC 
rem set CCWRAPDIR=%SRCDIR%\ccwrap
rem set INCOPTS=-I%CCWRAPDIR%\ow -I%FKSDIR% -I%SRCDIR%
set INCOPTS=-I%FKSDIR% -I%SRCDIR%

set RelDbg=%1
set OPTS=-ox -DNDEBUG
if /I "%RelDbg%"=="debug" set OPTS=-D_DEBUG
set OPTS=%OPTS% -DFKS_OLD_CXX

wcl386 -cc++ -xs -fe=abx.exe %INCOPTS% %OPTS% %FKSOPTS% %SRCS% %FKSSRCS% %AddLib%

del *.obj

endlocal
