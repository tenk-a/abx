#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <ctype.h>
#include "fil.h"


#define strend(p)	((p)+strlen(p))

/*---------------------------------------------------------------------------*/
static int  FSrh_recFlg = 1;		// 1:再帰する 0:しない
static int  FSrh_atr = 0x3f;		// 検索ﾌｧｲﾙ属性
static int  FSrh_nomalFlg = 1;		// ﾉｰﾏﾙ･ﾌｧｲﾙにﾏｯﾁ 1:する 0:しない
static char FSrh_fpath[FIL_NMSZ*3];
static char FSrh_fname[16];
static int (*FSrh_func)(char *path, char *t, FIL_FIND *ff);

static int FSrh_FindAndDo_Sub(void)
{
	FIL_FIND ff;
	char *t;

	t = strend(FSrh_fpath);
	strcpy(t,FSrh_fname);
	if (FIL_FindFirst(FSrh_fpath, FSrh_atr, &ff) == 0) {
		do {
			*t = '\0';
			if (FSrh_nomalFlg == 0 && (FSrh_atr & ff.attrib) == 0)
				continue;
			if (ff.name[0] != '.')
				FSrh_func(FSrh_fpath, t, &ff);
		} while (FIL_FindNext(&ff) == 0);
	}

	if (FSrh_recFlg) {
		strcpy(t,"*.*");
		if (FIL_FindFirst(FSrh_fpath, 0x10, &ff) == 0) {
			do {
				*t = '\0';
				if ((ff.attrib & 0x10) && ff.name[0] != '.') {
					strcpy(t, ff.name);
					strcat(t, "\\");
					FSrh_FindAndDo_Sub();
				}
			} while (FIL_FindNext(&ff) == 0);
		}
	}
	return 0;
}

int FSrh_FindAndDo(char *path, int atr, int recFlg, int knjFlg,
				 int (*fun)(char *apath, char *t, FIL_FIND *aff))
{
	char *p;

	FSrh_func   = fun;
	FSrh_recFlg = recFlg;
	FSrh_atr    = atr;
	FSrh_nomalFlg = 0;
	if (atr & 0x100) {
		atr &= 0xff;
		FSrh_nomalFlg = 1;
	}
	FIL_SetZenStat(knjFlg);
	FIL_FullPath(path, FSrh_fpath);
	p = strend(FSrh_fpath);
	if (p[-1] == ':' || p[-1] == '\\' || p[-1] == '/')
		strcat(FSrh_fpath, "*");
	p = FIL_BaseName(FSrh_fpath);
	strncpy(FSrh_fname, p, 15);
	*p = 0;
	return FSrh_FindAndDo_Sub();
}

/*---------------------------------------------------------------------------*/


int PrintFname(char *fpath, char *t, FIL_FIND *ff)
{
	strcat(t, ff->name);
	printf("%s\n",fpath);
	*t = '\0';
	return 0;
}

volatile void Usage(void)
{
	puts(
		"usage : frp [-opts] filename(s) [= 文字列]\n"
		" -r    サブディレクトリも探しにゆく\n"
		" -j    MS全角を考慮する  -j- しない\n"
		" -an   ノーマル属性のファイルを探す\n"
		" -ar   Read Only 属性のファイルを探す\n"
		" -ah   Hidden 属性のファイルを探す\n"
		" -as   System 属性のファイルを探す\n"
		" -av   ボリューム名を探す\n"
		" -ad   ディレクトリ属性のファイルを探す\n"
		" -aa   アーカイブ属性のファイルを探す\n"
		"\n"
		" -a[nrhsda] の指定のないばあい -anrhsa が指定されたことになる\n"
		" また、デフォルトで -j も指定されている\n"
		);
		
	exit(1);
}

int main(int argc, char *argv[])
{
	int i,c;
	char *p;
	int recFlg = 0;
	int knjFlg = 1;
	int atr = 0;

	if (argc < 2)
		Usage();
	for (i = 1; i < argc; i++) {
		p = argv[i];
		if (*p != '-')
			continue;
		p++; c = *p++; c = toupper(c);
		switch (c) {
		case 'J':
			knjFlg = 1;
			if (*p == '-')
				knjFlg = 0;
			break;
		case 'R':
			recFlg = 1;
			if (*p == '-')
				recFlg = 0;
			break;
		case 'A':
			strupr(p);
			while (*p) {
				switch(*p) {
				case 'N': atr |= 0x100; break;
				case 'R': atr |= 0x001; break;
				case 'H': atr |= 0x002; break;
				case 'S': atr |= 0x004; break;
				case 'V': atr |= 0x008; break;
				case 'D': atr |= 0x010; break;
				case 'A': atr |= 0x020; break;
				}
				++p;
			}
			break;
		case '?':
		case '\0':
			Usage();
			break;
		default:
			printf("bad option %s\n", argv[i]);
			return 1;
		}
	}
	if (atr == 0) {
		atr = 0x127;
	}
	for (i = 1; i < argc; i++) {
		p = argv[i];
		if (*p == '-')
			continue;
		FSrh_FindAndDo(p, atr, recFlg, knjFlg, PrintFname);
	}
	return 0;
}
