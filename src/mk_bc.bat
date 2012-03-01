del *.obj
rem BCC32 -Ox -d abx.c tree.c subr.c
BCC32 -N -v -y abx.c tree.c subr.c
copy abx.exe ..\abx.exe
del  abx.exe
del *.obj
del *.tds
