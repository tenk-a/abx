#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include "fil.h"



/*---------------------------------------------------------------------------*/
void Foo(char *name)
{
	FIL_FIND ff;
	char fname[FIL_NMSZ];
	char *p;

	FIL_FullPath(name, fname); //strcpy(fname, argv[1]);
	 //D printf("%s -> %s\n", name, fname);
	if (FIL_FindFirst(fname, 0x3f, &ff) == 0) {
		p = FIL_BaseName(fname);
		 //D printf("+ %s\n",p);
		do {
			strcpy(p, ff.name);
			if (ff.name[0] != '.')
				printf("%s\n", fname);
		} while (FIL_FindNext(&ff) == 0);
	};




/*---------------------------------------------------------------------------*/
volatile void Usage(void)
{
	puts("usage : frp [-opts] filename\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	int c,i;

	if (argc < 2)
		Usage();

	FIL_FullPath(argv[1], fname); //strcpy(fname, argv[1]);
	 //D printf("%s -> %s\n", argv[1], fname);
	if (FIL_FindFirst(fname, 0x3f, &ff) == 0) {
		p = FIL_BaseName(fname);
		 //D printf("+ %s\n",p);
		do {
			strcpy(p, ff.name);
			if (ff.name[0] != '.')
				printf("%s\n", fname);
		} while (FIL_FindNext(&ff) == 0);
	};
	return 0;
}
