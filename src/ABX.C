#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <dir.h>
#include <process.h>
#include "fil.h"

#define ABX	"ABX"

/*---------------------------------------------------------------------------*/
#define strend(p)	((p)+strlen(p))

void GetTmpDir(char *t)
{
	char *p;
	char nm[FIL_NMSZ];

	if (*t) {
		p = stpcpy(nm, t);
	} else {
		p = getenv("TMP");
		if (p == NULL) {
			p = getenv("TEMP");
			if (p == NULL) {
				p = ".\\";
			}
		}
		p = stpcpy(nm, p);
	}
	if (p[-1] != '\\' && p[-1] != ':' && p[-1] != '/')
		strcat(nm,"\\");
	strcat(nm,"*.*");
	FIL_FullPath(nm, t);
	p = FIL_BaseName(t);
	*p = 0;
	if (p[-1] == '\\')
		p[-1] = 0;
}


/*---------------------------------------------------------------------------*/
static int  FSrh_recFlg = 1;		// 1:再帰する 0:しない
static int  FSrh_atr = 0x3f;		// 検索ﾌｧｲﾙ属性
static int  FSrh_nomalFlg = 1;		// ﾉｰﾏﾙ･ﾌｧｲﾙにﾏｯﾁ 1:する 0:しない
static char FSrh_fpath[FIL_NMSZ*3];
static char FSrh_fname[16];
static int (*FSrh_func)(char *path /* , char *t, FIL_FIND *ff */);

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
			if (ff.name[0] != '.') {
			  #if 1
				strcpy(t, ff.name);
				FSrh_func(FSrh_fpath/*, t, &ff*/);
				*t = 0;
			  #else
				FSrh_func(FSrh_fpath, t, &ff);
			  #endif
			}
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
				 int (*fun)(char *apath/*, char *t, FIL_FIND *aff*/))
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
	FIL_SetZenMode(knjFlg);
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
#define CC_FMTSIZ	(0x4000U)
static char CC_drv[4];
static char CC_dir[FIL_NMSZ];
static char CC_name[40];
static char CC_ext[6];
FILE *CC_fp;
char CC_tmpDir[FIL_NMSZ];
char CC_v[10][FIL_NMSZ];
int  CC_vn = 0;
char CC_obuf[CC_FMTSIZ+0x800];
char CC_fmtBuf[CC_FMTSIZ];


int CC_Write(char *fpath /*, char *fname, FIL_FIND *ff*/)
{

	/* strcpy(fname, ff->name); */
	FIL_SplitPath(fpath, CC_drv, CC_dir, CC_name, CC_ext);
	/* *fname = 0; */

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
		if (CC_ext[0] == '.') {
			memmove(CC_ext, CC_ext+1,4);
		}
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
					if (c >= '1' && c <= '9') {
						p = stpcpy(p, CC_v[c-'0']);
					} else {
						fprintf(stderr,"Incorrect '$' format : '$%c'\n",c);
						/*fprintf(stderr,"."ABX"中 $指定がおかしい(%c)\n",c);*/
						exit(1);
					}
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
	printf(
		"\nバッチ生成支援 " ABX " v0.60\n"
		"    指定ﾌｧｲﾙ名を検索し, 該当ﾌｧｲﾙ各々に対し某かのｺﾏﾝﾄﾞを実行するﾊﾞｯﾁを生成する\n"
		"usage : %s [ｵﾌﾟｼｮﾝ] ﾌｧｲﾙ名 [=変換文字列]\n"
		,exename);
	puts(
		"ｵﾌﾟｼｮﾝ:                       ""変換文字:            変換例:\n"
		" -x[-] ﾊﾞｯﾁ実行する -x-しない   ""$f ﾌﾙﾊﾟｽ(拡張子付)   d:\\dir\\dir2\\filename.ext\n"
		" -r[-] ﾃﾞｨﾚｸﾄﾘ再帰する          ""$g ﾌﾙﾊﾟｽ(拡張子無)   d:\\dir\\dir2\\filename\n"
		" -an   nomal 属性を検索         ""$v ﾄﾞﾗｲﾌﾞ            d:\n"
		" -ar   Read Only 属性を検索     ""$p ﾃﾞｨﾚｸﾄﾘ(ﾄﾞﾗｲﾌﾞ付) d:\\dir\\dir2\n"
		" -ah   Hidden 属性を検索        ""$d ﾃﾞｨﾚｸﾄﾘ(ﾄﾞﾗｲﾌﾞ無) \\dir\\dir2\n"
		" -as   System 属性を検索        ""$c ﾌｧｲﾙ(拡張子付)    filename.ext\n"
		" -ad   ﾃﾞｨﾚｸﾄﾘ属性を検索        ""$x ﾌｧｲﾙ(拡張子無)    filename\n"
		" -aa   ｱｰｶｲﾌﾞ属性を検索         ""$e 拡張子            ext\n"
		" -b[-] 先頭にecho off付加       ""$w ﾃﾝﾎﾟﾗﾘ･ﾃﾞｨﾚｸﾄﾘ    (環境変数TMPの内容)\n"
		" -o<FILE> 出力ﾌｧｲﾙ指定          ""$1～$9 ｺﾏﾝﾄﾞﾗｲﾝで$指定された文字列\n"
		" -i<DIR>  検索ﾃﾞｨﾚｸﾄﾘ指定       ""$$ $ そのもの\n"
		" -w<DIR>  ﾃﾝﾎﾟﾗﾘ･ﾃﾞｨﾚｸﾄﾘ指定    ""$n 改行\n"
		" -e<EXT>  ﾃﾞﾌｫﾙﾄ拡張子指定      ""$t タブ\n"
		" @RESFILE ﾚｽﾎﾟﾝｽﾌｧｲﾙ入力        ""$s 空白\n"
		" +ABXFILE ."ABX"定義ﾌｧｲﾙ指定      ""\n"
		" :変換名  ."ABX"で定義した変換    ""\n"
		" $文字列  変換時$1～$9と置換    ""\n"
		/*" -j  全角対応(ﾃﾞﾌｫﾙﾄ)           "*/
		/*" -j- 全角未対応                 "*/
		/*" -av ボリューム名にﾏｯﾁ          "*/
		/*"\n"*/
		/*" -a[nrhsda]の指定のない時, -anrhsaが指定される\n"*/
		);
		
	exit(1);
}

/*---------------------------------------------------------------------------*/
static int  Opt_recFlg = 0;
static int  Opt_knjFlg = 1;
static int  Opt_atr = 0;
static int  Opt_batFlg = 0;
static int  Opt_batEx = 0;
static char Opt_outname[FIL_NMSZ] = "";
static char Opt_ipath[FIL_NMSZ] = "";
static char *Opt_iname = Opt_ipath;
static char Opt_abxName[FIL_NMSZ] = "";
static char *Opt_dfltExt = NULL;


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
	case 'X':
		Opt_batFlg = 1;
		if (*p == '-')
			Opt_batFlg = 0;
		break;
	case 'B':
		Opt_batEx = 1;
		if (*p == '-')
			Opt_batEx = 0;
		break;
	case 'E':
		Opt_dfltExt = p;
		break;
	case 'O':
		if (*p == 0)
			goto ERR_OPTS;
		strcpy(Opt_outname,p);
		break;
	case 'I':
		if (*p == 0)
			goto ERR_OPTS;
		FIL_FullPath(p,Opt_ipath);
		p = strend(Opt_ipath);
		if (p[-1] != '\\' && p[-1] != '/') {
			*p++ = '\\';
			*p = '\0';
		}
		Opt_iname = p;
		break;
	case 'W':
		strcpy(CC_tmpDir,p);
		GetTmpDir(CC_tmpDir);
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
	  #if 1
		printf("コマンドラインでのオプション指定がおかしい : %s\n", s);
	  #else
		printf("Incorrect command line option : %s\n", s);
	  #endif
		exit(1);
	}
}

/*---------------------------------------------------------------------------*/
static char E_files[0x1000];
static char *E_filep = E_files;
static size_t E_len = 0;

void E_AddFile(char *s)
{
	E_len += strlen(s) + 1;
	if (E_len >= (sizeof E_files)) {
		printf ("ﾚｽﾎﾟﾝｽﾌｧｲﾙ中に指定されたﾌｧｲﾙ名が多すぎる(%sでﾊﾞｯﾌｧがあふれた)\n",s);
		exit(1);
	}
	E_filep = stpcpy(E_filep, s) + 1;
	*E_filep = '\0';
}

/*---------------------------------------------------------------------------*/
static char Res_nm[FIL_NMSZ];
char *Res_p = CC_obuf;

char *Res_GetLine(void)
{
	char *p;

	p = Res_p;
	Res_p = strchr(p, '\n');
	if (Res_p != NULL) {
		if (Res_p[-1] == '\r')
			Res_p[-1] = 0;
		*Res_p++ = 0;
	} else {
		Res_p = strend(p);
	}
	return p;
}

void Res_GetFmts(void)
{
	char *p;
	while ( *(p = Res_GetLine()) != 0 ) {
		while ((p = strtok(p," \t\r\n")) != NULL) {
			if (*p == '#') {
				goto NEXT_LINE;
			} else if (*p == '-') {
				Opts(p);
			} else if (*p == '=') {
				memcpy(CC_fmtBuf, Res_p, strlen(Res_p)+1);
				goto RET;
			} else {
				E_AddFile(p);
			}
			p = NULL;
		}
	  NEXT_LINE:;
	}
  RET:;
}

void GetResFile(char *name)
{
	FILE *fp;
	unsigned l;

	strcpy(Res_nm,name);
	fp = fopen(Res_nm,"r");
	if (fp == NULL) {
	  #if 1
		printf("%s : ファイルをオープンできません\n", Res_nm);
	  #else
		printf("%s file not found.\n",Res_nm);
	  #endif
		exit(1);
	}
	l = fread(CC_obuf, 1, (sizeof CC_obuf)-2, fp);
	CC_obuf[l] = 0;
	fclose(fp);
	if (l == 0)
		return;

	Res_p = CC_obuf;
	Res_GetFmts();
}

#if 0
int Res_StrCmp(char *f, char *p)
{
	return strcmp(f,p);
}
#else

int Res_StrCmp(char *key, char *lin)
{
	char *k,*f;
	int  l;

	for (l = 0; l < 10; l++)
		CC_v[l][0] = 0;
	CC_vn = 1;

	k = key;
	f = lin;
	for (; ;) {
	  NEXT:
		if (*k == *f) {
			if (*k == '\0')
				return 0;	/* マッチしたぞ */
			k++;
			f++;
			continue;
		}
		if (*f == '{') {
			f++;
			do {
				l = strcspn(f,"|}");
				/*printf("l=%d\n",l);*/
				if (l < 1 || l >= (sizeof CC_v[0])-1)
					goto ERR1;
				if (memcmp(k,f,l) == 0) {
					if (CC_vn >= 10) {
						printf("%s のある検索行に{..}が10個以上ある %s\n",
								Res_nm,lin);
						exit(1);
					}
					memcpy(CC_v[CC_vn], f, l);
					CC_v[CC_vn++][l+1] = 0;
					k += l;
					f = strchr(f,'}');
					if (f == NULL) {
			  ERR1:
						printf("%s で{..}の指定がおかしい %s\n",Res_nm, lin);
						exit(1);
					}
					f++;
					goto NEXT;
				}
				f += l + 1;
			} while (f[-1] == '|');
		}
		break;
	}
	return -1;			/* マッチしなかった */

}
#endif

void GetCfgFile(char *name, char *key)
{
	FILE *fp;
	unsigned l;
	char *p;

	FIL_FullPath(name, Res_nm);
	fp = fopen(Res_nm,"r");
	if (fp == NULL) {
	  #if 1
		printf("%s : ファイルをオープンできません\n", Res_nm);
	  #else
		printf("%s file not found.\n",Res_nm);
	  #endif
		exit(1);
	}
	l = fread(CC_obuf, 1, (sizeof CC_obuf)-2, fp);
	CC_obuf[l] = 0;
	fclose(fp);
	if (l == 0)
		return;

	/*   */
	strupr(key);
	Res_p = CC_obuf;
	while ((Res_p = strstr(Res_p, "\n:")) != NULL) {
		Res_p ++;
		p = Res_GetLine();
		p = strtok(p, " \t\r");
		/*printf("cmp %s vs %s\n",key,p);*/
		if (p == NULL || *p == 0)
			continue;
		strupr(p);
		if (Res_StrCmp(key, p) == 0) {
			if ((p = strstr(Res_p, "\n:")) != NULL) {
				*p = '\0';
			}
			Res_GetFmts();
			return;
		}
	}
	printf("%s には %s は定義されていない\n", Res_nm, key);
	exit(1);
}


int main(int argc, char *argv[])
{
	int i,f;
	char *p;

	strlwr(strcpy(exename, FIL_BaseName(argv[0])));
	if (argc < 2)
		Usage();

	CC_tmpDir[0] = 0;
	GetTmpDir(CC_tmpDir);

	FIL_ChgExt(strcpy(Opt_abxName,argv[0]), ABX);
	for (i = 0; i < 10; i++)
		CC_v[i][0] = 0;
	CC_vn = 1;

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

		} else if (*p == '-') {
			Opts(p);

		} else if (*p == '@') {
			GetResFile(p+1);

		} else if (*p == '+') {
			++p;
			if (*p == '\\' || *p == '/' || p[1] == ':') {
				FIL_FullPath(p, Opt_abxName);
			} else {
				strcpy(CC_obuf, argv[0]);
				strcpy(FIL_BaseName(CC_obuf), p);
				FIL_FullPath(CC_obuf, Opt_abxName);
			}
			FIL_AddExt(Opt_abxName, ABX);

		} else if (*p == ':') {
			GetCfgFile(Opt_abxName, p);

		} else if (*p == '$') {
			if (CC_vn >= 10) {
				printf("$ 指定が多すぎる\n");
				exit(1);
			}
			strcpy(CC_v[CC_vn++], p+1);

	  #if 1
		} else {
			E_AddFile(p);
	  #endif
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
		strcat(Opt_outname,"\\_bx_tmp_.bat");
	}
	
	/* 出力ファイル設定 */
	if (Opt_outname[0]) {
		if ((CC_fp = fopen(Opt_outname, "w")) == NULL) {
		  #if 1
			printf("%s : ファイルをオープンできません\n", Opt_outname);
		  #else
			printf("%s : file open error\n", Opt_outname);
		  #endif
			return 1;
		}
	} else {
		CC_fp = stdout;
	}

	if (Opt_batEx)			/* バッチ実行用に先頭に echo off を置く */
		fprintf(CC_fp, "echo off\n");

	/* 実行 */
  #if 1
	for (p = E_files; *p; p = strend(p)+1) {
		if (*p != '\\' && *p != '/' && p[1] != ':') {
			strcpy(Opt_iname, p);
			strcpy(Opt_abxName, Opt_ipath);
		} else {
			strcpy(Opt_abxName, p);
		}
		FIL_AddExt(Opt_abxName, Opt_dfltExt);
		FSrh_FindAndDo(Opt_abxName, Opt_atr, Opt_recFlg, Opt_knjFlg, CC_Write);
	}
  #else
	for (i = 1; i < argc; i++) {
		p = argv[i];
		if (*p == '-')
			continue;
		if (*p != '\\' && *p != '/' && p[1] != ':') {
			strcpy(Opt_iname, p);
			strcpy(Opt_abxName, Opt_ipath);
		} else {
			strcpy(Opt_abxName, p);
		}
		FIL_AddExt(Opt_abxName, Opt_dfltExt);
		FSrh_FindAndDo(Opt_abxName, Opt_atr, Opt_recFlg, Opt_knjFlg, CC_Write);
	}
	for (p = E_files; *p; p = strend(p)+1) {
		if (*p != '\\' && *p != '/' && p[1] != ':') {
			strcpy(Opt_iname, p);
			strcpy(Opt_abxName, Opt_ipath);
		} else {
			strcpy(Opt_abxName, p);
		}
		FIL_AddExt(Opt_abxName, Opt_dfltExt);
		FSrh_FindAndDo(Opt_abxName, Opt_atr, Opt_recFlg, Opt_knjFlg, CC_Write);
	}
  #endif

	if (Opt_batEx)	/* バッチ実行用にファイル末に:ENDを付加する */
		fprintf(CC_fp, ":END\n");
	if (Opt_outname[0])
		fclose(CC_fp);

	/* バッチ実行のとき */
	if (Opt_batFlg) {
		p = getenv("COMSPEC");
		i = execl(p,p,"/c",Opt_outname,NULL);
	}

	return 0;
}
