call delbak .
call delobj .
lha u -t -a -x -r abx%1 *.*
lha d abx%1 lup.bat *.zzz *.abx
