ASM = tasm /ML /M
CC = TCC -mc -a- -G -1 -f- -p

.c.obj:
	$(CC) -c $<
.asm.obj:
	$(ASM) $*,$*,NUL,NUL

# TC4‚Ì‚Î‚©‚â‚ë`^^;
LIBS =	noehc.lib

OBJS =	\
	abx.obj 	tree.obj	fil.obj

abx.exe : $(OBJS)
	$(CC) $(OBJS) $(LIBS)

abx.obj  	: abx.c
tree.obj	: tree.c
fil.obj 	: fil.asm
