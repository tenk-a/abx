#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <dir.h>
#include <process.h>
#include "fil.h"


/*---------------------------------------------------------------------------*/
#define strend(p)	((p)+strlen(p))

char *FIL_ChgExt(char filename[], char *ext)
{
	char *p;

	if (filename[0] == '.') {
		filename ++;
	}
	if (filename[0] == '.') {
		filename ++;
	}
	p = strrchr(filename, '/');
	if ( p == NULL) {
		p = filename;
	}
	p = strrchr(p, '\\');
	if ( p == NULL) {
		p = filename;
	}
	p = strrchr( p, '.');
	if (p == NULL) {
		strcat(filename,".");
		strcat( filename, ext);
	} else {
		strcpy(p+1, ext);
	}
	return filename;
}

char *FIL_AddExt(char filename[], char *ext)
{
	char *p;

	if (filename[0] == '.') {
		filename ++;
	}
	if (filename[0] == '.') {
		filename ++;
	}
	p = strrchr(filename, '/');
	if ( p == NULL) {
		p = filename;
	}
	p = strrchr(p, '\\');
	if ( p == NULL) {
		p = filename;
	}
	if ( strrchr( p, '.') == NULL) {
		strcat(filename,".");
		strcat(filename, ext);
	}
	return filename;
}


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
#define CC_FMTSIZ	(0x7000U)
char CC_fmtBuf[CC_FMTSIZ];
char *CC_p = CC_fmtBuf;
static char CC_drv[4];
static char CC_dir[FIL_NMSZ];
static char CC_name[40];
static char CC_ext[6];
char CC_tmpDir[FIL_NMSZ];
FILE *CC_fp;

static char CC_obuf[CC_FMTSIZ+0x800];


int CC_Write(char *fpath, char *fname, FIL_FIND *ff)
{

	strcpy(fname, ff->name);
	FIL_SplitPath(fpath, CC_drv, CC_dir, CC_name, CC_ext);
	*fname = 0;

	{
		int l;
		char *p;
		/* ディレクトリ名の後ろの'\'をはずす */
		l = strlen(CC_dir);
		if (l) {
			p = CC_dir + l - 1;
			if (*p == '\\')
				*p = 0;
		}
		/* 拡張子の '.' をはずす */
		memmove(CC_ext, CC_ext+1,4);
	}

	{
		char c,*p,*s;
		s = CC_fmtBuf;
		p = CC_obuf;
		while ((c = (*p++ = *s++)) != '\0') {
			if (c == '$') {
				--p;
				c = *s++;
				c = toupper(c);
				switch (c) {
				case '$':	*p++ = c;		break;
				case 'S':	*p++ = ' '; 	break;
				case 'T':	*p++ = '\t';	break;
				case 'N':	*p++ = '\n';	break;
				case 'V':	p = stpcpy(p,CC_drv);	break;
				case 'D':	p = stpcpy(p,CC_dir);	break;
				case 'X':	p = stpcpy(p,CC_name);	break;
				case 'E':	p = stpcpy(p,CC_ext);	break;
				case 'W':	p = stpcpy(p,CC_tmpDir);	break;

				case 'P':
					p = stpcpy(p,CC_drv);
					p = stpcpy(p,CC_dir);
					break;
				case 'C':
					p = stpcpy(p,CC_name);
					if (CC_ext[0]) {
						p = stpcpy(p,".");
						p = stpcpy(p,CC_ext);
					}
					break;
				case 'F':
					p = stpcpy(p,CC_drv);
					p = stpcpy(p,CC_dir);
					p = stpcpy(p,"\\");
					p = stpcpy(p,CC_name);
					if (CC_ext[0]) {
						p = stpcpy(p,".");
						p = stpcpy(p,CC_ext);
					}
					break;
				case 'G':
					p = stpcpy(p,CC_drv);
					p = stpcpy(p,CC_dir);
					p = stpcpy(p,"\\");
					p = stpcpy(p,CC_name);
					break;
				default:
					fprintf(stderr,"bad format '$%c'\n",c);
					/*fprintf(stderr,"変換文字列中の$指定がおかしい(%c)\n",c);*/
					exit(1);
				}
			}
		}
	}

	fprintf(CC_fp, "%s", CC_obuf);
	return 0;
}



/*---------------------------------------------------------------------------*/
static char exename[FIL_NMSZ];

volatile void Usage(void)
{
	puts("バッチ生成支援プログラム frp v0.50\n");
	printf("usage : %s [ｵﾌﾟｼｮﾝ] filename(s) [=変換文字列]\n",exename);
	puts(
		"ｵﾌﾟｼｮﾝ:                     ""変換文字:            変換例\n"
		" -r  ﾃﾞｨﾚｸﾄﾘ再帰             ""$f ﾌﾙﾊﾟｽ(拡張子付)   d:\dir\dir2\filename.ext\n"
		" -j  全角対応(ﾃﾞﾌｫﾙﾄ)        ""$g ﾌﾙﾊﾟｽ(拡張子無)   d:\dir\dir2\filename\n"
		" -j- 全角未対応              ""$v ﾄﾞﾗｲﾌﾞ            d:\n"
		" -an nomal 属性にﾏｯﾁ         ""$p ﾃﾞｨﾚｸﾄﾘ(ﾄﾞﾗｲﾌﾞ付) d:\dir\dir2\n"
		" -ar Read Only 属性にﾏｯﾁ     ""$d ﾃﾞｨﾚｸﾄﾘ(ﾄﾞﾗｲﾌﾞ無) \dir\dir2\n"
		" -ah Hidden 属性にﾏｯﾁ        ""$c ﾌｧｲﾙ(拡張子付)    file.ext\n"
		" -as System 属性にﾏｯﾁ        ""$x ﾌｧｲﾙ(拡張子無)    filename\n"
		" -ad ﾃﾞｨﾚｸﾄﾘ属性にﾏｯﾁ        ""$e 拡張子            ext\n"
		" -aa ｱｰｶｲﾌﾞ属性にﾏｯﾁ         ""$w ﾃﾝﾎﾟﾗﾘ･ﾃﾞｨﾚｸﾄﾘ    環境変数TMPの内容\n"
		" -b  ﾊﾞｯﾁ実行                ""$$ $そのもの         $\n"
		" -o<path> 出力先指定         ""$n 改行\n"
		" @resfile ﾚｽﾎﾟﾝｽﾌｧｲﾙ入力     ""$t タブ\n"
		"                             ""$s 空白\n"
		/*" -av ボリューム名にﾏｯﾁ   \n"*/
		"\n"
		" -a[nrhsda]の指定のない時, -anrhsaが指定される\n"
		);
		
	exit(1);
}

static int  Opt_recFlg = 0;
static int  Opt_knjFlg = 1;
static int  Opt_atr = 0;
static char Opt_outname[FIL_NMSZ] = "";
static int  Opt_batFlg = 0;

void Opts(char *s)
{
	char c,*p;

	p = s;
	p++; c = *p++; c = toupper(c);
	switch (c) {
	case 'J':
		Opt_knjFlg = 1;
		if (*p == '-')
			Opt_knjFlg = 0;
		break;
	case 'R':
		Opt_recFlg = 1;
		if (*p == '-')
			Opt_recFlg = 0;
		break;
	case 'O':
		if (*p == 0)
			goto ERR_OPTS;
		strcpy(Opt_outname,p);
		break;
	case 'B':
		Opt_batFlg = 1;
		break;
	case 'A':
		strupr(p);
		while (*p) {
			switch(*p) {
			case 'N': Opt_atr |= 0x100; break;
			case 'R': Opt_atr |= 0x001; break;
			case 'H': Opt_atr |= 0x002; break;
			case 'S': Opt_atr |= 0x004; break;
			case 'V': Opt_atr |= 0x008; break;
			case 'D': Opt_atr |= 0x010; break;
			case 'A': Opt_atr |= 0x020; break;
			}
			++p;
		}
		break;
	case '?':
	case '\0':
		Usage();
		break;
	default:
  ERR_OPTS:
		printf("bad option %s\n", s);
		exit(1);
	}
}

char *Res_GetLine(void)
{
	char *p;

	p = CC_p;
	CC_p = strchr(p, '\n');
	if (CC_p != NULL) {
		if (CC_p[-1] == '\r')
			CC_p[-1] = 0;
		*CC_p++ = 0;
	} else {
		CC_p = strend(p);
	}
	return p;
}

void GetResFile(char *name)
{
	FILE *fp;
	unsigned l;
	char *p;
	char nm[FIL_NMSZ];

	if (name == NULL || *name == 0) {
		printf("Unable to open file.\n");
		exit(1);
	}
	FIL_AddExt(strcpy(nm,name), "FRP");
	fp = fopen(nm,"rb");
	if (fp == NULL) {
		printf("%s : file not found.\n",nm);
		exit(1);
	}
	l = fread(CC_fmtBuf, 1, (sizeof CC_fmtBuf)-2, fp);
	CC_fmtBuf[l] = 0;
	fclose(fp);
	if (l == 0)
		return;
	CC_p = CC_fmtBuf;

	/*   */
	while ( *(p = Res_GetLine()) != 0 ) {
		while ((p = strtok(p," \t\r\n")) != NULL) {
			if (*p == ';') {
				goto NEXT_LINE;
			} else if (*p == '-') {
				Opts(p);
			} else if (*p == '=') {
				goto RET;
			}
			p = NULL;
		}
	  NEXT_LINE:;
	}
  RET:
	memmove(CC_fmtBuf, CC_p, strlen(CC_p)+1);
}

void GetTmpDir(char *t)
{
	char *p;
	char nm[FIL_NMSZ];

	p = getenv("TMP");
	if (p == NULL) {
		p = getenv("TEMP");
		if (p == NULL) {
			p = ".\\";
		}
	}
	p = stpcpy(nm, p);
	if (p[-1] != '\\' && p[-1] != ':' && p[-1] != '/')
		strcat(nm,"\\");
	strcat(nm,"*.*");
	FIL_FullPath(nm, t);
	p = FIL_BaseName(t);
	*p = 0;
	if (p[-1] == '\\')
		p[-1] = 0;
}


int main(int argc, char *argv[])
{
	int i,f;
	char *p;

	strlwr(strcpy(exename, FIL_BaseName(argv[0])));
	if (argc < 2)
		Usage();

	GetTmpDir(CC_tmpDir);

	for (f = i = 1; i < argc; i++) {
		p = argv[i];
		if (f == 0) {
		  LLL1:
			strcat(CC_fmtBuf, p);
			strcat(CC_fmtBuf, " ");

		} else if (*p == '=') {
			f = 0;
			p++;
			if (*p)
				goto LLL1;

		} else if (*p == '@') {
			GetResFile(p+1);
		} else if (*p == '-') {
			Opts(p);
		}
	}
	if (Opt_atr == 0) {
		Opt_atr = 0x127;
	}

	/* 変換文字列調整 */
	if (CC_fmtBuf[0] == '\0')
		strcpy(CC_fmtBuf, "$f\n");
	p = strchr(CC_fmtBuf, '\n');
	if (p == NULL)
		strcat(CC_fmtBuf, "\n");

	/* バッチ実行のとき */
	if (Opt_batFlg) {
		strcpy(Opt_outname, CC_tmpDir);
		strcat(Opt_outname,"\\frp_tmp_.bat");
	}
	
	/* 出力ファイル設定 */
	if (Opt_outname[0]) {
		if ((CC_fp = fopen(Opt_outname, "w")) == NULL) {
			printf("%s : file open error\n", Opt_outname);
			return 1;
		}
		if (Opt_batFlg)
			fprintf(CC_fp, "echo off\n");
	} else {
		CC_fp = stdout;
	}

	/* 実行 */
	for (i = 1; i < argc; i++) {
		p = argv[i];
		if (*p == '-')
			continue;
		FSrh_FindAndDo(p, Opt_atr, Opt_recFlg, Opt_knjFlg, CC_Write);
	}
	if (Opt_outname[0])
		fclose(CC_fp);
	
	/* バッチ実行 */
	if (Opt_batFlg) {
		p = getenv("COMSPEC");
		if (p == NULL) {
			system(Opt_outname);
		} else {
			i = execl(p,p,"/c",Opt_outname,NULL);
		}
	}
	
	return 0;
}
