rem TCC v4.0用
rem TCC -mc -a- -G -1 -f- -p abx.c tree.c fil.asm noehc.lib

rem v4.0 以前の場合は noehc.libは不要
rem TCC -mc -a- -G -1 -f- -p abx.c tree.c fil.asm

BCC32i abx.c tree.c fil.c
