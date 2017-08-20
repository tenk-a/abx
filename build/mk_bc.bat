del *.obj
BCC32 -N -v -y -Dintptr_t=int -Dstrtoull=strtoul ..\src\abx.c ..\src\tree.c ..\src\subr.c
del *.obj
del *.tds
