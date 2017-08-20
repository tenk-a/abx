rem BCC v4.5—p
del *.obj
BCC -mc -a- -G -1 -f- -Ox -p -DC16 -eabx16.exe abx.c tree.c subr.c fil.asm noehc.lib
copy abx16.exe ..\abx16.exe
del  abx16.exe
