rem BCC v4.5—p
cd ..\src
del *.obj
BCC -mc -a- -G -1 -f- -Ox -p -DC16 -eabx16.exe abx.c tree.c subr.c fil.asm noehc.lib
del *.obj
cd ..\build
