copy abx.zzz \up\abx.zzz
copy abxg.zzz \up\abxg.zzz
rem lha u -t -z abxsrc abx.c tree.c tree.h fil.h fil.asm mk.bat
lha u -t \up\abx%1 abx.exe abx.doc abxg.cfg abxg.doc abx.his delbak.bat lzhdate.bat
