#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <dir.h>
#include <process.h>
#include "fil.h"
#include "tree.h"

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
			  #if 0
				p = ".\\";
			  #else
				printf("環境変数TMPかTEMP, または-w<DIR>でﾃﾝﾎﾟﾗﾘ･ﾃﾞｨﾚｸﾄﾘを指定してください\n");
				exit(1);
			  #endif
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
static long FSrh_topN,FSrh_topCnt;
static int  FSrh_topFlg;
static char FSrh_fpath[FIL_NMSZ*3];
static char FSrh_fname[16];
static unsigned long FSrh_szMin;
static unsigned long FSrh_szMax;
static unsigned short FSrh_dateMin;
static unsigned short FSrh_dateMax;
static int (*FSrh_func)(char *path /* , char *t, FIL_FIND *ff */);


#if 1  /* -s ソート関係 */

static int  FSrh_sortFlg = 0, FSrh_sortRevFlg = 0;

static void *FSrh_New(void/*FIL_FIND*/ *ff)
{
	void/*FIL_FIND*/ *p;
	p = malloc(sizeof (FIL_FIND));
	if (p == NULL) {
		printf("メモリが足りません\n");
		exit(1);
	}
	memcpy(p, ff, sizeof(FIL_FIND));
	return p;
}

static void FSrh_Del(void/*FIL_FIND*/ *ff)
{
	free (ff);
}

static int  FSrh_NamCmp(FIL_FIND *f1, FIL_FIND *f2)
{
	return strcmp(f1->name, f2->name);
}

static int  FSrh_Cmp(FIL_FIND *f1, FIL_FIND *f2)
{
	int n;

	if (FSrh_sortFlg <= 1) {						/* 名前でソート */
		n = strcmp(f1->name, f2->name);
		if (FSrh_sortRevFlg)
			return -n;
		return n;
	}

	if (FSrh_sortFlg == 0x02) {						/* 拡張子 */
		char *p,*q;
		p = strrchr(f1->name, '.');
		p = (p == NULL) ? "" : p;
		q = strrchr(f2->name, '.');
		q = (q == NULL) ? "" : q;
		n = strcmp(p,q);

	} else if (FSrh_sortFlg == 0x04) {				/* サイズ */
		long t;
		t = f1->size - f2->size;
		n = (t > 0) ? 1 : (t < 0) ? -1 : 0;

	} else if (FSrh_sortFlg == 0x08) {				/* 時間 */
		long t;
		t = (long)f1->wr_date - (long)f2->wr_date;
		n = (t > 0) ? 1 : (t < 0) ? -1 : 0;
		if (n == 0) {
			t = (long)f1->wr_time - (long)f2->wr_time;
			n = (t > 0) ? 1 : (t < 0) ? -1 : 0;
		}
	} else if (FSrh_sortFlg == 0x10) {				/* 属性 */
		/* アーカイブ属性は邪魔なのでオフする */
		n = ((int)f2->attrib & 0xDF) - ((int)f1->attrib & 0xDF);
	} 

	if (n == 0) {
		n = strcmp(f1->name, f2->name);
		if (FSrh_sortRevFlg)
			n = -n;
	}
	if (FSrh_sortRevFlg)
		return -n;
	return n;
}


static void *FSrh_Malloc(unsigned siz)
{
	void *p;

	p = malloc(siz);
	if (p == NULL) {
		printf("メモリ不足です\n");
		exit(1);
	}
	return p;
}

static void FSrh_DoOne(void *ff)
{
	char *t;

	if (FSrh_topFlg) {
		if (FSrh_topCnt == 0)		/* 先頭 N個のみの処理のとき */
			return;
		else
			--FSrh_topCnt;
	}
	t = strend(FSrh_fpath);
	strcpy(t, ((FIL_FIND*)ff)->name);
	FSrh_func(FSrh_fpath);
	*t = 0;
}

static void FSrh_DoOneDir(void *ff)
{
	char *t;
	int FSrh_FindAndDo_SubSort(void);

	t = strend(FSrh_fpath);
	strcpy(t, ((FIL_FIND*)ff)->name);
	strcat(t, "\\");
	FSrh_FindAndDo_SubSort();
	*t = 0;
}

static int FSrh_FindAndDo_SubSort(void)
{
	FIL_FIND ff;
	char *t;
	TREE *tree;

	if (FSrh_topFlg) {
		FSrh_topCnt = FSrh_topN;
	}
	tree = TREE_Make(FSrh_New, FSrh_Del, (TREE_CMP)FSrh_Cmp, FSrh_Malloc);
	t = strend(FSrh_fpath);
	strcpy(t,FSrh_fname);
	if (FIL_FindFirst(FSrh_fpath, FSrh_atr, &ff) == 0) {
		do {
			*t = '\0';
			if (FSrh_nomalFlg == 0 && (FSrh_atr & ff.attrib) == 0)
				continue;
			if(	 (ff.name[0] != '.')
			  && (	(FSrh_szMin > FSrh_szMax) || (FSrh_szMin <= ff.size && ff.size <= FSrh_szMax)	)
			  && (	(FSrh_dateMin > FSrh_dateMax) || (FSrh_dateMin <= ff.wr_date && ff.wr_date <= FSrh_dateMax) )
			  )
			{
				TREE_Insert(tree, &ff);
			}
		} while (FIL_FindNext(&ff) == 0);
	}
	TREE_DoAll(tree, FSrh_DoOne);
	TREE_Clear(tree);

	if (FSrh_recFlg) {
		tree = TREE_Make(FSrh_New, FSrh_Del, (TREE_CMP)FSrh_NamCmp, FSrh_Malloc);
		strcpy(t,"*.*");
		if (FIL_FindFirst(FSrh_fpath, 0x10, &ff) == 0) {
			do {
				*t = '\0';
				if ((ff.attrib & 0x10) && ff.name[0] != '.') {
					TREE_Insert(tree, &ff);
				}
			} while (FIL_FindNext(&ff) == 0);
		}
		TREE_DoAll(tree, FSrh_DoOneDir);
		TREE_Clear(tree);
	}
	return 0;
}

#endif


static int FSrh_FindAndDo_Sub(void)
{
	FIL_FIND ff;
	char *t;

	if (FSrh_topFlg) {
		FSrh_topCnt = FSrh_topN;
	}
	t = strend(FSrh_fpath);
	strcpy(t,FSrh_fname);
	if (FIL_FindFirst(FSrh_fpath, FSrh_atr, &ff) == 0) {
		do {
			*t = '\0';
			if (FSrh_nomalFlg == 0 && (FSrh_atr & ff.attrib) == 0)
				continue;
			if(	 (ff.name[0] != '.')
			  && (	(FSrh_szMin > FSrh_szMax) || (FSrh_szMin <= ff.size && ff.size <= FSrh_szMax)	)
			  && (	(FSrh_dateMin > FSrh_dateMax) || (FSrh_dateMin <= ff.wr_date && ff.wr_date <= FSrh_dateMax) )
			  )
			{
				strcpy(t, ff.name);
				FSrh_func(FSrh_fpath);
				*t = 0;
				if (FSrh_topFlg && --FSrh_topCnt == 0) {	/* 先頭 N個のみの処理のとき */
					return 0;
				}
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
				long topn, int sortFlg,
				unsigned long szmin, unsigned long szmax,
				unsigned short dtmin, unsigned short dtmax,
				int (*fun)(char *apath/*, char *t, FIL_FIND *aff*/))
{
	char *p;

	FSrh_func   = fun;
	FSrh_recFlg = recFlg;
	FSrh_atr    = atr;
	FSrh_topN	= topn;
	FSrh_topFlg = (topn != 0);
	FSrh_szMin  = szmin;
	FSrh_szMax  = szmax;
	FSrh_dateMin = dtmin;
	FSrh_dateMax = dtmax;
	/*printf("%lu(%lx)-%lu(%lx)\n",szmin,szmin,szmax,szmax);*/
	/*printf("date %04x-%04x\n",dtmin,dtmax);*/
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
	if (sortFlg) {
		FSrh_sortRevFlg = (sortFlg & 0x80);
		FSrh_sortFlg = sortFlg & 0x7f;
		return FSrh_FindAndDo_SubSort();
	}
	return FSrh_FindAndDo_Sub();
}

/*---------------------------------------------------------------------------*/
#define CC_FMTSIZ	(0x4000U)
static char CC_drv[4];
static char CC_dir[FIL_NMSZ];
static char CC_name[40];
static char CC_ext[6];
FILE *CC_fp;
int  CC_lwrFlg = 0;
char CC_tmpDir[FIL_NMSZ];
int  CC_vn = 0;
char CC_v[10][FIL_NMSZ];
int  CC_no[10];
char CC_obuf[CC_FMTSIZ+0x800];
char CC_fmtBuf[CC_FMTSIZ];
char CC_pathDir[FIL_NMSZ];
char CC_chgPathDir[FIL_NMSZ];

int CC_Write(char *fpath /*, char *fname, FIL_FIND *ff*/)
{
	/* strcpy(fname, ff->name); */
	FIL_SplitPath(fpath, CC_drv, CC_dir, CC_name, CC_ext);
	/* *fname = 0; */

	if (CC_lwrFlg) {
		strlwr(CC_drv);
		strlwr(CC_dir);
		strlwr(CC_name);
		strlwr(CC_ext);
	}

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
		strcpy(CC_pathDir,CC_drv);
		strcat(CC_pathDir,CC_dir);
		if (CC_chgPathDir[0]) {
			strcpy(CC_pathDir, CC_chgPathDir);
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
				case '[':	*p++ = '<';		break;
				case ']':	*p++ = '>';		break;
				case 'S':	*p++ = ' '; 	break;
				case 'T':	*p++ = '\t';	break;
				case 'N':	*p++ = '\n';	break;
				case 'V':	*p++ = CC_drv[0]; *p = '\0';	break;
				case 'D':	p = stpcpy(p,CC_dir);	break;
				case 'X':	p = stpcpy(p,CC_name);	break;
				case 'E':	p = stpcpy(p,CC_ext);	break;
				case 'W':	p = stpcpy(p,CC_tmpDir);	break;

				case 'P':
					p = stpcpy(p,CC_pathDir);
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
						/*fprintf(stderr,".CFG 中 $指定がおかしい(%c)\n",c);*/
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
		"\nバッチ生成支援 ABX v2.00                                      by てんかﾐ☆\n"
		"    指定ﾌｧｲﾙ名を検索し, 該当ﾌｧｲﾙ各々に対し某かのｺﾏﾝﾄﾞを実行するﾊﾞｯﾁを生成する\n"
		"usage : %s [ｵﾌﾟｼｮﾝ] ﾌｧｲﾙ名 [=変換文字列]\n"
		,exename);
	printf("%s",
		"ｵﾌﾟｼｮﾝ:                        ""変換文字:            変換例:\n"
		" -x[-]    ﾊﾞｯﾁ実行   -x-しない "" $f ﾌﾙﾊﾟｽ(拡張子付)   d:\\dir\\dir2\\filename.ext\n"
		" -r[-]    ﾃﾞｨﾚｸﾄﾘ再帰          "" $g ﾌﾙﾊﾟｽ(拡張子無)   d:\\dir\\dir2\\filename\n"
		" -a[nrhsd] 指定ﾌｧｲﾙ属性で検索  "" $v ﾄﾞﾗｲﾌﾞ            d\n"
		"          n:一般    r:ﾘｰﾄﾞｵﾝﾘｰ "" $p ﾃﾞｨﾚｸﾄﾘ(ﾄﾞﾗｲﾌﾞ付) d:\\dir\\dir2\n"
		"          h:隠し    s:ｼｽﾃﾑ     "" $d ﾃﾞｨﾚｸﾄﾘ(ﾄﾞﾗｲﾌﾞ無) \\dir\\dir2\n"
		"          d:ﾃﾞｨﾚｸﾄﾘ            "" $c ﾌｧｲﾙ(拡張子付)    filename.ext\n"
		" -z[N-M]  ｻｲｽﾞN～Mのものを検索 "" $x ﾌｧｲﾙ(拡張子無)    filename\n"
		" -d[A-B]  日付A～Bのものを検索 "" $e 拡張子            ext\n"
		" -s[nezta][r] ｿｰﾄ(整列)        "" $w ﾃﾝﾎﾟﾗﾘ･ﾃﾞｨﾚｸﾄﾘ    (環境変数TMPの内容)\n"
		"          n:名 e:拡張子 z:ｻｲｽﾞ "" $$ $     $n 改行\n"
		"          t:日付 a:属性 r:降順 "" $t タブ  $s 空白\n"
		" -b[-]    先頭にecho off付加   "" $[ <     $] >\n"
		" -l[-]    ﾌｧｲﾙ名を小[大]文字化 ""----------------------------------------------\n"
		" -t[N]    最初のN個のみ処理    "" -p<DIR>  $pを強制的に変更     ""\n"
		" -e<EXT>  ﾃﾞﾌｫﾙﾄ拡張子指定     "" @RESFILE ﾚｽﾎﾟﾝｽﾌｧｲﾙ入力       ""\n"
		" -w<DIR>  ﾃﾝﾎﾟﾗﾘ･ﾃﾞｨﾚｸﾄﾘ指定   "" +CFGFILE .CFG 定義ﾌｧｲﾙ指定    ""\n"
		" -o<FILE> 出力ﾌｧｲﾙ指定         "" :変換名  .CFG で定義した変換  ""\n"
		" -i<DIR>  検索ﾃﾞｨﾚｸﾄﾘ指定      "" :        :変換名一覧を表示    ""\n"
		/*" -a[nrhsda]の指定のない時, -anrhsaが指定される\n"*/
		/*" -j  全角対応(ﾃﾞﾌｫﾙﾄ)           "*/
		/*" -j- 全角未対応                 "*/
		/*" -aa   ｱｰｶｲﾌﾞ属性を検索         "*/
		/*" -av ボリューム名にﾏｯﾁ          "*/
		/*"\n"*/
		);

	exit(1);
}

/*---------------------------------------------------------------------------*/
static int  Opt_recFlg = 0;
static int  Opt_knjFlg = 1;
static int  Opt_atr = 0;
static int  Opt_batFlg = 0;
static int  Opt_batEx = 0;
static int  Opt_sort = 0;
static long Opt_topN = 0;
static char Opt_outname[FIL_NMSZ] = "";
static char Opt_ipath[FIL_NMSZ] = "";
static char *Opt_iname = Opt_ipath;
static char Opt_abxName[FIL_NMSZ] = "";
static char Opt_dfltExt[6] = "";
static char *Opt_dfltExtp = NULL;
static unsigned long Opt_szmin = 0xFFFFFFFFUL;	/* szmin > szmaxのとき比較を行わない*/
static unsigned long Opt_szmax = 0UL;
static unsigned short Opt_dtmin = 0xFFFFU;		/* dtmin > dtmaxのとき比較を行わない*/
static unsigned short Opt_dtmax = 0;

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
	case 'L':
		CC_lwrFlg = 1;
		if (*p == '-')
			CC_lwrFlg = 0;
		break;
	case 'T':
		if (*p == 0) {
			Opt_topN = 1;
		} else {
			Opt_topN = strtol(p,NULL,0);
		}
		break;
	case 'E':
		Opt_dfltExtp = strncpy(Opt_dfltExt, p, 4);
		if (*p == '$' && p[1] >= '1' && p[1] <= '9' && p[2] == 0) {
			strncpy(Opt_dfltExt, CC_v[p[1]-'0'], 4);
		}
		Opt_dfltExt[3] = 0;
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
	case 'P':
		if (*p == 0)
			goto ERR_OPTS;
		FIL_FullPath(p,CC_chgPathDir);
		p = strend(CC_chgPathDir);
		if (p[-1] == '\\' || p[-1] == '/') {
			p[-1] = '\0';
		}
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
	case 'S':
		c = 0;
		Opt_sort = 0x01;
		strupr(p);
		while (*p) {
			switch(*p) {
			case '-': Opt_sort = 0x00; break;
			case 'N': Opt_sort = 0x01; break;
			case 'E': Opt_sort = 0x02; break;
			case 'Z': Opt_sort = 0x04; break;
			case 'T': Opt_sort = 0x08; break;
			case 'A': Opt_sort = 0x10; break;
			case 'R': c = 0x80;        break;
			}
			++p;
		}
		Opt_sort |= c;
		break;
	case 'Z':
		Opt_szmin = (*p == '-') ? 0 : strtoul(p, &p, 0);
		if (*p == 'K' || *p == 'k')			p++, Opt_szmin *= 1024UL;
		else if (*p == 'M' || *p == 'm')	p++, Opt_szmin *= 1024UL*1024UL;
		if (*p) { /* *p == '-' */
			Opt_szmax = 0xffffffffUL;
			p++;
			if (*p) {
				Opt_szmax = strtoul(p,&p,0);
				if (*p == 'K' || *p == 'k')			p++, Opt_szmax *= 1024UL;
				else if (*p == 'M' || *p == 'm')	p++, Opt_szmax *= 1024UL*1024UL;
			}
			if (Opt_szmax < Opt_szmin)
				goto ERR_OPTS;
		} else {
			Opt_szmax = Opt_szmin;
		}
		break;
	case 'D':
		if (*p == 0) {
			Opt_dtmax = Opt_dtmin = 0;
		} else {
			unsigned long t;
			int y,m,d;
			t = (*p == '-') ? 0 : strtoul(p,&p,10);
			y = (int)((t/10000) % 100);	y = (y >= 80) ? (y-80) : (y+100-80);
			m = (int)((t / 100) % 100);	if (m == 0 || 12 < m) goto ERR_OPTS;
			d = (int)(t % 100);			if (d == 0 || 31 < d) goto ERR_OPTS;
			Opt_dtmin = (y<<9)|(m<<5)|d;
			if (*p) {
				p++;
				Opt_dtmax = 0xFFFFU;
				if (*p) {
					t = strtoul(p,&p,10);
					y = (int)(t/10000)%100;	y = (y>=80) ? (y-80) : (y+100-80);
					m = (int)(t/100) % 100;	if (m==0 || 12 < m) goto ERR_OPTS;
					d = (int)(t % 100);		if (d==0 || 31 < d) goto ERR_OPTS;
					Opt_dtmax = (y<<9)|(m<<5)|d;
					if (Opt_dtmax < Opt_dtmin)
						goto ERR_OPTS;
				}
			} else {
				Opt_dtmax = Opt_dtmin;
			}
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
static char E_files[0x4000];
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

char *Res_SetDoll(char *p0)
{
	int m,n,i;
	char *p;

	p = p0;
	m = *p++;	if (m < '1' || m > '9') goto ERR;
	m -= '0';
	if (*p++ != ':') goto ERR;
	n = *p++;	if (n < '1' || n > '9') goto ERR;
	n -= '0';
	if (*p++ != '{') goto ERR;
	i = 0;
	do {
		int l;
		l = strcspn(p,"|}");
		if (l < 1 || l >= (sizeof CC_v[0])-1)
			goto ERR;
		if (i == CC_no[n]) {
			memcpy(CC_v[m], p, l);
			CC_v[m][l+1] = 0;
			p = strchr(p,'}'); if (p == NULL) goto ERR;
			p++;
			goto RET;
		}
		i++;
		p += l + 1;
	} while (p[-1] == '|');
  ERR:
	printf(".CFG ファイルで $Ｎ:Ｍ{..}指定でおかしいものがある : $%s\n",p0);
	exit(1);
  RET:
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
		  #if 10
			} else if (*p == '$') {
				p = Res_SetDoll(p+1);
		  #endif
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
			CC_no[CC_vn] = 0;
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
				CC_no[CC_vn]++;
			} while (f[-1] == '|');
			CC_no[CC_vn] = 0;
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

	if (key[1] == 0) /* ':'だけの指定のとき */
		printf("':変換名'一覧\n");
	l = 1;
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
		if (key[1]) {
			if (Res_StrCmp(key, p) == 0) {
				if ((p = strstr(Res_p, "\n:")) != NULL) {
					*p = '\0';
				}
				Res_GetFmts();
				return;
			}
		} else {
			printf("\t%s\n",p);
			if (++l == 23) {
				printf("[more]");
				DOS_KbdIn();
				printf("\b\b\b\b\b\b      \b\b\b\b\b\b");
				l = 0;
			}
		}
	}
	if (key[1])
		printf("%s には %s は定義されていない\n", Res_nm, key);
	exit(1);
}


int cdecl main(int argc, char *argv[])
{
	int i,f;
	char *p;

	strlwr(strcpy(exename, FIL_BaseName(argv[0])));
	if (argc < 2)
		Usage();

	CC_tmpDir[0] = 0;
	/*GetTmpDir(CC_tmpDir);*/

	FIL_ChgExt(strcpy(Opt_abxName,argv[0]), "CFG");
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
			FIL_AddExt(Opt_abxName, "CFG");

		} else if (*p == ':') {
			if (p[1] == '#') {
				printf(":#で始まる文字列は指定できません（%s）\n",p);
				exit(1);
			}
			GetCfgFile(Opt_abxName, p);
	  #if 0
		} else if (*p == '$') {
			if (CC_vn >= 10) {
				printf("$ 指定が多すぎる\n");
				exit(1);
			}
			strcpy(CC_v[CC_vn++], p+1);
	  #endif
	  #if 1
		} else {
			E_AddFile(p);
	  #endif
		}
	}

	if (CC_tmpDir[0] == 0)
		GetTmpDir(CC_tmpDir);
	
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
		strcat(Opt_outname,"\\_abx_tmp.bat");
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
		FIL_AddExt(Opt_abxName, Opt_dfltExtp);
		FSrh_FindAndDo(Opt_abxName, Opt_atr, Opt_recFlg, Opt_knjFlg,Opt_topN,
			Opt_sort, Opt_szmin, Opt_szmax, Opt_dtmin, Opt_dtmax, CC_Write);
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
		FIL_AddExt(Opt_abxName, Opt_dfltExtp);
		FSrh_FindAndDo(Opt_abxName, Opt_atr, Opt_recFlg, Opt_knjFlg,Opt_topN,
			Opt_sort, Opt_szmin, Opt_szmax, Opt_dtmin, Opt_dtmax, CC_Write);
	}
	for (p = E_files; *p; p = strend(p)+1) {
		if (*p != '\\' && *p != '/' && p[1] != ':') {
			strcpy(Opt_iname, p);
			strcpy(Opt_abxName, Opt_ipath);
		} else {
			strcpy(Opt_abxName, p);
		}
		FIL_AddExt(Opt_abxName, Opt_dfltExtp);
		FSrh_FindAndDo(Opt_abxName, Opt_atr, Opt_recFlg, Opt_knjFlg,Opt_topN,
			Opt_sort, Opt_szmin, Opt_szmax, Opt_dtmin, Opt_dtmax, CC_Write);
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
		/* ※ これ以降は実行されない */
	}

	return 0;
}
