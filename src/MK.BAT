rem BCC v4.5—p

rem ana -DTURBO_C -ms -1 -c2 -o. fil.ana
BCC -mc -a- -G -1 -f- -Ox -p abx.c tree.c fil.asm noehc.lib
copy abx.exe ..\abx.exe
del abx.exe
