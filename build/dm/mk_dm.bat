@echo off
setlocal

set SRCDIR=..\..\src
set FKSDIR=%SRCDIR%\fks
set SRCS=%SRCDIR%\abx.cpp %SRCDIR%\AbxConvFmt.cpp %SRCDIR%\AbxFiles.cpp %SRCDIR%\AbxMsgStr.cpp %SRCDIR%\AbxMsgStrJp.cpp
set FKSSRCS=%FKSDIR%\fks_path.c %FKSDIR%\fks_io.c %FKSDIR%\fks_io_mbs.cpp %FKSDIR%\fks_mbc.c %FKSDIR%\fks_misc.c %FKSDIR%\fks_dirent.c %FKSDIR%\fks_time.c
set FKSOPTS=%FKSOPTS% -DFKS_UNUSE_WIN32_PATHMATCHSPEC -DFKS_NO_SHLWAPI_H -DFKS_USE_OLD_WIN32_API
rem set FKSOPTS=%FKSOPTS% -DFKS_SRC_DBC
set CCWRAPDIR=%SRCDIR%\ccwrap
set INCOPTS=-I%CCWRAPDIR%\dmc -I%FKSDIR% -I%SRCDIR%
rem set INCOPTS=-I%FKSDIR% -I%SRCDIR%

set RelDbg=%1
set OPTS=-DNDEBUG
if /I "%RelDbg%"=="debug" set OPTS=-D_DEBUG -g
set OPTS=%OPTS% -D_CONSOLE

dmc -oabx.exe -DFKS_OLD_CXX %INCOPTS% %OPTS% %FKSOPTS% %SRCS% %FKSSRCS% %AddLib%

del *.obj

endlocal
