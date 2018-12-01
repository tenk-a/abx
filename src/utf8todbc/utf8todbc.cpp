/**
 *	@file utf8todbc.cpp
 *  @brief  utf8 -> dbc text
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @license Boost Software License Version 1.0
 */
#include <fks_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <fks_io.h>
#include <fks_io_mbs.h>

int main(int argc, char* argv[])
{
	char* 	fname;
	char*	m;
	char*	b;
	size_t	l;

	if (argc < 2) {
		fprintf(stderr, "usage> utf8todbc.exe utf8textfile\n");
		return 1;
	}
	fks_ioMbsInit(1,0);
	fname = argv[1];
	l = fks_fileSize(fname);
	m = (char*)calloc(1, l + 1024);
	if (!m) {
		fprintf(stderr, "ERROR: Not enough memory.");
		return 1;
	}
	if (fks_fileLoad(fname, m, l, NULL) == NULL) {
		fprintf(stderr, "file '%s' read error.", fname);
		return 1;
	}
	b = (char*)calloc(1, l*6 + 1024);
	if (!b) {
		fprintf(stderr, "ERROR: Not enough memory.");
		return 1;
	}
	fks_ioMbsToOutput(b, l*6, m);
	printf("%s", b);
	free(m);
	free(b);
	return 0;
}
