/*
 	ABX v3.11
	2001-03  -ct の追加
	2001-09  -ct での、の追加
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <dir.h>
#include <process.h>
#include "subr.h"
#include "tree.h"

#ifdef C16
#define CC_OBUFSIZ		0x4000U		/* 定義ファイル等のサイズ				*/
#define CC_FMTSIZ		0x4000U		/* 定義ファイル等のサイズ				*/
#else
#define CC_OBUFSIZ		0x20000		/* 定義ファイル等のサイズ				*/
#define CC_FMTSIZ		0x20000		/* 定義ファイル等のサイズ				*/
#endif

/*---------------------------------------------------------------------------*/
static char exename[FIL_NMSZ];

volatile void Usage(void)
{
	printf(
	  #ifdef C16
		"ABX(msdos) v3.11  ﾌｧｲﾙ名を検索,該当ﾌｧｲﾙ名を文字列に埋込(ﾊﾞｯﾁ生成).  by tenk*\n"
	  #else
		"ABX(w95dos) v3.11 ﾌｧｲﾙ名を検索,該当ﾌｧｲﾙ名を文字列に埋込(ﾊﾞｯﾁ生成).  by tenk*\n"
	  #endif
		"usage : %s [ｵﾌﾟｼｮﾝ] ['変換文字列'] ﾌｧｲﾙ名 [=変換文字列]\n" ,exename);
	printf("%s",
		"ｵﾌﾟｼｮﾝ:                        ""変換文字:            変換例:\n"
		" -x[-]    ﾊﾞｯﾁ実行   -x-しない "" $f ﾌﾙﾊﾟｽ(拡張子付)   d:\\dir\\dir2\\filename.ext\n"
		" -r[-]    ﾃﾞｨﾚｸﾄﾘ再帰          "" $g ﾌﾙﾊﾟｽ(拡張子無)   d:\\dir\\dir2\\filename\n"
		" -n[-]    ﾌｧｲﾙ検索しない -n-有 "" $v ﾄﾞﾗｲﾌﾞ            d\n"
		" -a[nrhsd] 指定ﾌｧｲﾙ属性で検索  "" $p ﾃﾞｨﾚｸﾄﾘ(ﾄﾞﾗｲﾌﾞ付) d:\\dir\\dir2\n"
		"          n:一般 s:ｼｽﾃﾑ h:隠し "" $d ﾃﾞｨﾚｸﾄﾘ(ﾄﾞﾗｲﾌﾞ無) \\dir\\dir2\n"
		"          r:ﾘｰﾄﾞｵﾝﾘｰ d:ﾃﾞｨﾚｸﾄﾘ "" $c ﾌｧｲﾙ(拡張子付)    filename.ext\n"
		" -z[N-M]  ｻｲｽﾞN～MのFILEを検索 "" $x ﾌｧｲﾙ(拡張子無)    filename\n"
		" -d[A-B]  日付A～BのFILEを検索 "" $e 拡張子            ext\n"
		" -s[nezta][r] ｿｰﾄ(整列)        "" $w ﾃﾝﾎﾟﾗﾘ･ﾃﾞｨﾚｸﾄﾘ    (環境変数TMPの内容)\n"
		"          n:名 e:拡張子 z:ｻｲｽﾞ "" $z ｻｲｽﾞ(10進10桁)    1234567890 ※$Zなら16進8桁\n"
		"          t:日付 a:属性 r:降順 "" $j 時間              1993-02-14\n"
		" -t[N]    最初のN個のみ処理    "" $i 連番生成          ※$Iなら16進数\n"
		" -u[-]    $c|$Cでﾌｧｲﾙ名大小文字"" $$ $  $[ <  $` '  $n 改行  $t ﾀﾌﾞ\n"
		" -l[-]    @入力で名前は行単位  "" $# #  $] >  $^ \"  $s 空白  $l 生入力のまま\n"
		" -ci[N:M] N:$iの開始番号(M:終) ""------------------------------------------------\n"
		" -ct<FILE> FILEより新しいなら  ""-p<DIR>  $pの強制変更   ""-ck[-] 日本語名のみ検索\n"
		" +CFGFILE .CFGﾌｧｲﾙ指定         ""-e<EXT>  ﾃﾞﾌｫﾙﾄ拡張子   ""-cy[-] \\を含む全角名検索\n"
		" @RESFILE ﾚｽﾎﾟﾝｽﾌｧｲﾙ           ""-o<FILE> 出力ﾌｧｲﾙ指定    ""\n"
		" :変換名  CFGで定義した変換    ""-i<DIR>  検索ﾃﾞｨﾚｸﾄﾘ指定 ""\n"
		" :        変換名一覧を表示     ""-w<DIR>  TMPﾃﾞｨﾚｸﾄﾘ指定  ""\n"
		/*" -a[nrhsda]の指定のない時, -anrhsaが指定される\n"*/
		/*" -j  全角対応(ﾃﾞﾌｫﾙﾄ)          "*/
		/*" -j- 全角未対応                "*/
		/*" -aa   ｱｰｶｲﾌﾞ属性を検索        "*/
		/*" -av ボリューム名にﾏｯﾁ         "*/
		/*" -b       echo off を付加      "*/
		/* #begin,#end ﾚｽﾎﾟﾝｽﾌｧｲﾙ中,変換前後にﾃｷｽﾄ出力""\n"*/
		);

	exit(0);
}

/*---------------------------------------------------------------------------*/
static int  FSrh_recFlg = 1;		// 1:再帰する 0:しない
static int  FSrh_atr = 0x3f;		// 検索ﾌｧｲﾙ属性
static int  FSrh_nomalFlg = 1;		// ﾉｰﾏﾙ･ﾌｧｲﾙにﾏｯﾁ 1:する 0:しない
static int  FSrh_knjChk = 0;
static long FSrh_topN,FSrh_topCnt;
static int  FSrh_topFlg = 0;
static int  FSrh_nonFF = 0;			// 1:ファイル検索しない 0:する
static char FSrh_fpath[FIL_NMSZ * 3];
static char FSrh_fname[FIL_NMSZ+2];
static unsigned long FSrh_szMin;
static unsigned long FSrh_szMax;
static unsigned short FSrh_dateMin;
static unsigned short FSrh_dateMax;
static int (*FSrh_func)(char *path, FIL_FIND *ff);


static int FSrh_ChkKnjs(char *p)
{
	unsigned char c;
	while((c = *(unsigned char *)p++) != 0) {
		if (c & 0x80)
			return 1;
	}
	return 0;
}


#if 1  /* -s ソート関係 */

static int  FSrh_sortFlg = 0, FSrh_sortRevFlg = 0, FSrh_uplwFlg;

static void *FSrh_New(void/*FIL_FIND*/ *ff)
{
	void/*FIL_FIND*/ *p;
	p = (void*)malloc(sizeof (FIL_FIND));
	if (p == NULL) {
		printfE("メモリが足りません\n");
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


static void FSrh_DoOne(void *ff)
{
	char *t;

	if (FSrh_topFlg) {
		if (FSrh_topCnt == 0)		/* 先頭 N個のみの処理のとき */
			return;
		else
			--FSrh_topCnt;
	}
	t = STREND(FSrh_fpath);
	strcpy(t, ((FIL_FIND*)ff)->name);
	FSrh_func(FSrh_fpath, ff);
	*t = 0;
}

static void FSrh_DoOneDir(void *ff)
{
	char *t;
	int FSrh_FindAndDo_SubSort(void);

	t = STREND(FSrh_fpath);
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
	tree = TREE_Make(FSrh_New, FSrh_Del, (TREE_CMP)FSrh_Cmp, mallocE, freeE);
	t = STREND(FSrh_fpath);
	strcpy(t,FSrh_fname);
	if (FIL_FindFirst(FSrh_fpath, FSrh_atr, &ff) == 0) {
		do {
			*t = '\0';
			if (FSrh_nomalFlg == 0 && (FSrh_atr & ff.attrib) == 0)
				continue;
			if (FSrh_uplwFlg) {
				FIL_NameUpr(ff.name);
			}
			if(	 (ff.name[0] != '.')
			  && (	(FSrh_szMin > FSrh_szMax) || ((int)FSrh_szMin <= ff.size && ff.size <= (int)FSrh_szMax)	)
			  && (	(FSrh_dateMin > FSrh_dateMax) || (FSrh_dateMin <= ff.wr_date && ff.wr_date <= FSrh_dateMax) )
			  && (	(FSrh_knjChk==0) || (FSrh_knjChk==1 && FSrh_ChkKnjs(ff.name)) || (FSrh_knjChk==2 && strchr(ff.name,'\\'))
									 || (FSrh_knjChk==-1&& !FSrh_ChkKnjs(ff.name))|| (FSrh_knjChk==-2&& !strchr(ff.name,'\\'))	)
			  )
			{
				TREE_Insert(tree, &ff);
			}
		} while (FIL_FindNext(&ff) == 0);
	}
	TREE_DoAll(tree, FSrh_DoOne);
	TREE_Clear(tree);

	if (FSrh_recFlg /*&& FSrh_nonFF == 0*/) {
		tree = TREE_Make(FSrh_New, FSrh_Del, (TREE_CMP)FSrh_NamCmp, mallocE, freeE);
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
	t = STREND(FSrh_fpath);
	strcpy(t,FSrh_fname);
	if (FIL_FindFirst(FSrh_fpath, FSrh_atr, &ff) == 0) {
		do {
			*t = '\0';
			if (FSrh_nomalFlg == 0 && (FSrh_atr & ff.attrib) == 0)
				continue;
			if(	 (ff.name[0] != '.')
			  && (	(FSrh_szMin > FSrh_szMax) || ((int)FSrh_szMin <= ff.size && ff.size <= (int)FSrh_szMax)	)
			  && (	(FSrh_dateMin > FSrh_dateMax) || (FSrh_dateMin <= ff.wr_date && ff.wr_date <= FSrh_dateMax)	)
			  && (	(FSrh_knjChk==0) || (FSrh_knjChk==1 && FSrh_ChkKnjs(ff.name)) || (FSrh_knjChk==2 && strchr(ff.name,'\\'))
			  						 || (FSrh_knjChk==-1&& !FSrh_ChkKnjs(ff.name))|| (FSrh_knjChk==-2&& !strchr(ff.name,'\\'))	)
			  )
			{
				strcpy(t, ff.name);
				FSrh_func(FSrh_fpath, &ff);
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

int FSrh_FindAndDo(char *path, int atr, int recFlg, int zenFlg,
				long topn, int sortFlg, int knjChk, int nonFF,
				unsigned long szmin, unsigned long szmax,
				unsigned short dtmin, unsigned short dtmax,
				int (*fun)(char *apath, FIL_FIND *aff))
{
	char *p;

	FSrh_func   = fun;
	FSrh_recFlg = recFlg;
	FSrh_atr    = atr;
	FSrh_topN	= topn;
	FSrh_topFlg = (topn != 0);
	FSrh_knjChk = knjChk;
	FSrh_szMin  = szmin;
	FSrh_szMax  = szmax;
	FSrh_dateMin = dtmin;
	FSrh_dateMax = dtmax;
	FSrh_nonFF   = nonFF;
	FIL_SetZenMode(zenFlg);
	/*printf("%lu(%lx)-%lu(%lx)\n",szmin,szmin,szmax,szmax);*/
	/*printf("date %04x-%04x\n",dtmin,dtmax);*/
	FSrh_nomalFlg = 0;
	if (atr & 0x100) {
		atr &= 0xff;
		FSrh_atr = atr;
		FSrh_nomalFlg = 1;
	}
	FIL_FullPath(path, FSrh_fpath);
	p = STREND(FSrh_fpath);
	if (p[-1] == ':' || p[-1] == '\\' || p[-1] == '/')
		strcat(FSrh_fpath, "*");
	p = FIL_BaseName(FSrh_fpath);
	strncpyZ(FSrh_fname, p, FIL_NMSZ);
	if (FSrh_nonFF) {	/* ファイル検索しない場合 */
		FIL_FIND ff;
		memset(&ff, 0, sizeof ff);
		if ( (FSrh_knjChk==0) || (FSrh_knjChk==1 && FSrh_ChkKnjs(FSrh_fname)) || (FSrh_knjChk==2 && strchr(FSrh_fname,'\\'))
				|| (FSrh_knjChk==-1&& !FSrh_ChkKnjs(FSrh_fname))|| (FSrh_knjChk==-2&& !strchr(FSrh_fname,'\\'))	)
		{
			FSrh_func(FSrh_fpath, &ff);
		}
		return 0;
	}
	/* ファイル検索する場合 */
	*p = 0;
	if (sortFlg) {	/* ソートする */
		FSrh_sortRevFlg = (sortFlg & 0x80);
		FSrh_sortFlg = sortFlg & 0x7f;
		FSrh_uplwFlg = sortFlg & 0x8000;
		return FSrh_FindAndDo_SubSort();
	}
	return FSrh_FindAndDo_Sub();
}

/*---------------------------------------------------------------------------*/
static char CC_drv[4];
static char CC_dir[FIL_NMSZ];
static char CC_name[FIL_NMSZ];
static char CC_ext[6];
FILE *CC_fp;
/*int  CC_lwrFlg = 0;*/
#ifdef C16
int  CC_upLwrFlg = 1;
long CC_num;						/* $i で生成する番号 */
long CC_numEnd;						/* 連番をファイル名の文字列の代わりにする指定をした場合の終了アドレス */
#else
int  CC_upLwrFlg = 0;
int  CC_num;						/* $i で生成する番号 */
int  CC_numEnd;						/* 連番をファイル名の文字列の代わりにする指定をした場合の終了アドレス */
#endif
char CC_tmpDir[FIL_NMSZ];
int  CC_vn = 0;
char CC_v[10][FIL_NMSZ];
int  CC_no[10];
char CC_pathDir[FIL_NMSZ];
char CC_chgPathDir[FIL_NMSZ];
char CC_fmtBuf[CC_FMTSIZ];			/* 変換文字列を収める */
char CC_obuf[CC_OBUFSIZ+FIL_NMSZ];	/* .cfg(.res) 読み込みや、出力用のバッファ */
char CC_tgtnm[FIL_NMSZ+FIL_NMSZ];
char CC_tgtnmFmt[FIL_NMSZ+FIL_NMSZ];


char *CC_StpCpy(char *d, char *s, int clm, int flg)
{
	unsigned char c;
	int n;

	n = 0;
	if (CC_upLwrFlg == 0) {
		strcpy(d,s);
		n = strlen(s);
		d = d + n;
	} else if (flg == 0) {	/* 大文字化 */
		while ((c = *(unsigned char *)s++) != '\0') {
			if (islower(c))
				c = toupper(c);
			*d++ = (char)c;
			n++;
			if (ISKANJI(c) && *s && FIL_GetZenMode()) {
				*d++ = *s++;
				n++;
			}
		}
	} else {		/* 小文字化 */
		while ((c = *(unsigned char *)s++) != '\0') {
			if (isupper(c))
				c = tolower(c);
			*d++ = (char)c;
			n++;
			if (ISKANJI(c) && *s && FIL_GetZenMode()) {
				*d++ = *s++;
				n++;
			}
		}
	}
	clm -= n;
	while (clm > 0) {
		*d++ = ' ';
		--clm;
	}
	*d = '\0';
	return d;
}

static void CC_SplitPath(char *fpath)
{
	int l;
	char *p;

	FIL_SplitPath(fpath, CC_drv, CC_dir, CC_name, CC_ext);

  #if 0
 //	if (CC_lwrFlg) {
 //		strlwr(CC_drv);
 //		strlwr(CC_dir);
 //		strlwr(CC_name);
 //		strlwr(CC_ext);
 //	}
  #endif

  #if 1
	/* ディレクトリ名の後ろの'\'をはずす */
	l = strlen(CC_dir);
	if (l) {
		p = CC_dir + l - 1;
		if (*p == '\\')
			*p = 0;
	}
  #else
	FIL_DelLastDirSep(CC_dir);	/* ディレクトリ名の後ろの'\'をはずす */
  #endif
	strcpy(CC_pathDir,CC_drv);
	strcat(CC_pathDir,CC_dir);
	if (CC_chgPathDir[0]) {
		strcpy(CC_pathDir, CC_chgPathDir);
	}
	/* 拡張子の '.' をはずす */
	if (CC_ext[0] == '.') {
		memmove(CC_ext, CC_ext+1, strlen(CC_ext+1)+1);
	}
}

char *CC_lineBuf;
static void CC_StrFmt(char *dst, const char *src, int sz, FIL_FIND *ff);

int CC_Write(char *fpath, FIL_FIND *ff)
{
	CC_SplitPath(fpath);

	CC_StrFmt(CC_tgtnm, CC_tgtnmFmt, FIL_NMSZ, ff);					// 今回のターゲット名を設定
	if (CC_tgtnmFmt[0] == 0 || FIL_FdateCmp(CC_tgtnm, fpath) < 0) {	// 日付比較しないか、する場合はターゲットが古ければ
		CC_StrFmt(CC_obuf, CC_fmtBuf, CC_OBUFSIZ, ff);
		fprintf(CC_fp, "%s", CC_obuf);
	}
	CC_num++;
	return 0;
}


static void CC_StrFmt(char *dst, const char *src, int sz, FIL_FIND *ff)
{
	static char buf[FIL_NMSZ*4];
	char c, *b, *p, *pe;
	const char *s;
	int  f,n;
	char drv[2];
	drv[0] = CC_drv[0];
	drv[1] = 0;

	s = src;
	p = dst;
	pe = p + sz;
	while ((c = (*p++ = *s++)) != '\0' && p < pe) {
		if (c == '$') {
			--p;
			n = -1;
			c = *s++;
			if (c == '+') {	/* +NN は桁数指定だ */
				n = strtoul(s,(char**)&s,10);
				if (s == NULL || *s == 0)
					break;
				if (n >= FIL_NMSZ)
					n = FIL_NMSZ;
				c = *s++;
			}
			f = islower(c);
			switch (toupper(c)) {
			case 'S':	*p++ = ' '; 	break;
			case 'T':	*p++ = '\t';	break;
			case 'N':	*p++ = '\n';	break;
			case '$':	*p++ = '$';		break;
			case '#':	*p++ = '#';		break;
			case '[':	*p++ = '<';		break;
			case ']':	*p++ = '>';		break;
			case '`':	*p++ = '\'';	break;
			case '^':	*p++ = '"';		break;

			case 'L':	p = CC_StpCpy(p,CC_lineBuf,n,f);		break;
			case 'V':	p = CC_StpCpy(p,drv,n,f);		break;
			case 'D':	p = CC_StpCpy(p,CC_dir,n,f);	break;
			case 'X':	p = CC_StpCpy(p,CC_name,n,f);	break;
			case 'E':	p = CC_StpCpy(p,CC_ext,n,f);	break;
			case 'W':	p = CC_StpCpy(p,CC_tmpDir,n,f);	break;
			case 'P':	p = CC_StpCpy(p,CC_pathDir,n,f);break;

			case 'C':
				b = CC_StpCpy(buf,CC_name,0,f);
				if (CC_ext[0]) {
					b = stpcpy(b,".");
					/*b=*/CC_StpCpy(b,CC_ext,0,f);
				}
				if (n < 0) n = 1;
				p += sprintf(p, "%-*s", n, buf);
				break;
			case 'F':
				b = CC_StpCpy(buf,CC_drv,0,f);
				b = CC_StpCpy(b,CC_dir,0,f);
				b = stpcpy(b,"\\");
				b = CC_StpCpy(b,CC_name,0,f);
				if (CC_ext[0]) {
					b = stpcpy(b,".");
					/*b=*/ CC_StpCpy(b,CC_ext,0,f);
				}
				if (n < 0) n = 1;
				p += sprintf(p, "%-*s", n, buf);
				break;
			case 'G':
				b = CC_StpCpy(buf,CC_drv,0,f);
				b = CC_StpCpy(b,CC_dir,0,f);
				b = stpcpy(b,"\\");
				/*b =*/CC_StpCpy(b,CC_name,0,f);
				if (n < 0) n = 1;
				p += sprintf(p, "%-*s", n, buf);
				break;
			case 'O':
				CC_StpCpy(buf, CC_tgtnm, 0, f);
				if (n < 0) n = 1;
				p += sprintf(p, "%-*s", n, buf);
				break;
			case 'Z':
				if (f) {
					if (n < 0)
						n = 10;
				  #ifdef C16
					p += sprintf(p, "%*ld", n, (ULONG)ff->size);
				  #else
					p += sprintf(p, "%*d", n, ff->size);
				  #endif
				} else {
					if (n < 0)
						n = 8;
				  #ifdef C16
					p += sprintf(p, "%*LX", n, (ULONG)ff->size);
				  #else
					p += sprintf(p, "%*X", n, ff->size);
				  #endif
				}
				break;

			case 'I':
				if (f) {
					if (n < 0)
						n = 1;
				  #ifdef C16
					p += sprintf(p, "%0*ld", n, CC_num);
				  #else
					p += sprintf(p, "%0*d", n, CC_num);
				  #endif
				} else {
					if (n < 0)
						n = 1;
				  #ifdef C16
					p += sprintf(p, "%0*LX", n, CC_num);
				  #else
					p += sprintf(p, "%0*X", n, CC_num);
				  #endif
				}
				break;

			case 'J':
				{	int y,m,d;
					y = (1980+((unsigned short)ff->wr_date>>9));
					m = (ff->wr_date>>5) & 0x0f;
					d = (ff->wr_date   ) & 0x1f;
					if (n < 0)
						n = 10;
					if (n >= 10) {
						sprintf(buf, "%04d-%02d-%02d", y, m, d);
					} else if (n >= 8) {
						sprintf(buf, "%02d-%02d-%02d", y %100, m, d);
					} else {
						sprintf(buf, "%02d-%02d", m, d);
					}
					p += sprintf(p, "%-*s", n, buf);
				}
				break;
			default:
				if (c >= '1' && c <= '9') {
					p = stpcpy(p, CC_v[c-'0']);
				} else {
					fprintf(STDERR, "Incorrect '$' format : '$%c'\n",c);
					/*fprintfE(STDERR,".CFG 中 $指定がおかしい(%c)\n",c);*/
					exit(1);
				}
			}
		}
	}
}




int CC_WriteLine0(char *fmtBuf)
{
	char c,*p,*s;

	s = fmtBuf;
	p = CC_obuf;
	while ((c = (*p++ = *s++)) != '\0') {
		if (c == '$') {
			--p;
			c = *s++;
			if (c == '$') {
				*p++ = '$';
			} else if (c >= '1' && c <= '9') {
				p = stpcpy(p, CC_v[c-'0']);
			} else {
				fprintf(STDERR,"Incorrect '$' format : '$%c'\n",c);
				/*fprintfE(STDERR,"レスポンス中の $指定がおかしい(%c)\n",c);*/
				exit(1);
			}
		}
	}
	fprintf(CC_fp, "%s\n", CC_obuf);
	return 0;
}



/*--------------------------------------------------------------------------*/
static int  Opt_recFlg = 0;						/* 再帰の有無 */
static int  Opt_zenFlg = 1;						/* MS全角対応 */
static int  Opt_knjChk = 0;						/* MS全角存在チェック */
static int  Opt_atr    = 0;						/* ファイル属性 */
static int  Opt_batFlg = 0;						/* バッチ実行 */
static int  Opt_batEx  = 0;						/* -bの有無 */
static int  Opt_sort   = 0;						/* ソート */
static long Opt_topN   = 0;						/* 処理個数 */
static int  Opt_nonFF  = 0;						/* ファイル検索しない */
static int  Opt_linInFlg = 0;					/* RES入力を行単位処理*/
static char Opt_outname[FIL_NMSZ] = "";			/* 出力ファイル名 */
static char Opt_ipath[FIL_NMSZ]   = "";			/* 入力パス名 */
static char *Opt_iname = Opt_ipath;				/* 入力ファイル名 */
static char Opt_abxName[FIL_NMSZ] = "";			/* 名前 work */
static char Opt_dfltExt[FIL_NMSZ] = "";			/* デフォルト拡張子 */
static char *Opt_dfltExtp = NULL;				/* デフォルト拡張子 */
static unsigned long  Opt_szmin = 0xFFFFFFFFUL;	/* szmin > szmaxのとき比較を行わない*/
static unsigned long  Opt_szmax = 0UL;
static unsigned short Opt_dtmin = 0xFFFFU;		/* dtmin > dtmaxのとき比較を行わない*/
static unsigned short Opt_dtmax = 0;
static int  Opt_renbanStart;					/* 連番の開始番号. 普通0 */
static int  Opt_renbanEnd;						/* 連番の開始番号. 普通0 */



void Opts(char *s)
{
	char c,*p;

	p = s;
	p++; c = *p++; c = toupper(c);
	switch (c) {
	case 'X':
		Opt_batFlg = (*p != '-');
		break;
	case 'R':
		Opt_recFlg = (*p != '-');
		break;
	case 'U':
		CC_upLwrFlg = (*p != '-');
		break;
	case 'N':
		Opt_nonFF = (*p != '-');
		if (*p == 'd' || *p == 'D')
			Opt_nonFF = 2;
		break;
	case 'J':
		Opt_zenFlg = (*p != '-');
		break;
	case 'B':
		Opt_batEx = (*p != '-');
		break;
	case 'L':
		Opt_linInFlg  = (*p != '-');
		break;
  #if 0
 //	case 'L':
 //		CC_lwrFlg = (*p != '-');
 //		break;
  #endif
	case 'T':
		if (*p == 0) {
			Opt_topN = 1;
		} else {
			Opt_topN = strtol(p,NULL,0);
		}
		break;
	case 'C':
		c = toupper(*p);
		if (c == '-') {
			Opt_knjChk = 0;
		} else if (c == 'K') {
			Opt_knjChk = 1;
			if (p[1] == '-')
				Opt_knjChk = -1;
		} else if (c == 'Y') {
			Opt_knjChk = 2;
			if (p[1] == '-')
				Opt_knjChk = -2;
		} else if (c == 'T' || c == 'F') {	// 'F'は旧互換
			strncpyZ(CC_tgtnmFmt, p+1, FIL_NMSZ);
		} else if (c == 'I') {
			Opt_renbanStart = strtol(p+1, &p, 0);
			if (*p) {
				Opt_renbanEnd = strtol(p+1, &p, 0);
			} else {
				Opt_renbanEnd = 0;
			}
		} else {
			goto ERR_OPTS;
		}
		break;
	case 'E':
		Opt_dfltExtp = strncpyZ(Opt_dfltExt, p, FIL_NMSZ);
		if (*p == '$' && p[1] >= '1' && p[1] <= '9' && p[2] == 0) {
			strcpy(Opt_dfltExt, CC_v[p[1]-'0']);
		}
		/*Opt_dfltExt[3] = 0;*/
		break;
	case 'O':
		if (*p == 0)
			goto ERR_OPTS;
		strcpy(Opt_outname,p);
		break;
	case 'I':
		if (*p == 0)
			goto ERR_OPTS;
		FIL_FullPath(p, Opt_ipath);
		p = STREND(Opt_ipath);
		if (p[-1] != '\\' && p[-1] != '/') {
			*p++ = '\\';
			*p = '\0';
		}
		Opt_iname = p;
		break;
	case 'P':
		if (*p == 0)
			goto ERR_OPTS;
		FIL_FullPath(p, CC_chgPathDir);
		p = STREND(CC_chgPathDir);
		if (p[-1] == '\\' || p[-1] == '/') {
			p[-1] = '\0';
		}
		break;
	case 'W':
		strcpy(CC_tmpDir,p);
		FIL_GetTmpDir(CC_tmpDir);
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
			if (*p == '-') {
				Opt_dtmin = 0;
			} else {
				t = strtoul(p,&p,10);
				y = (int)((t/10000) % 100);	y = (y >= 80) ? (y-80) : (y+100-80);
				m = (int)((t / 100) % 100);	if (m == 0 || 12 < m) goto ERR_OPTS;
				d = (int)(t % 100);			if (d == 0 || 31 < d) goto ERR_OPTS;
				Opt_dtmin = (y<<9)|(m<<5)|d;
			}
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
		printfE("コマンドラインでのオプション指定がおかしい : %s\n", s);
	  #else
		printfE("Incorrect command line option : %s\n", s);
	  #endif
	}
}

/*---------------------------------------------------------------------------*/
SLIST_T	*fileListTop    = NULL;
SLIST_T	*beforeTextList = NULL;
SLIST_T	*afterTextList  = NULL;
static char Res_nm[FIL_NMSZ];
char *Res_p = CC_obuf;

char *Res_GetLine(void)
	/* CC_obufに貯えたテキストより１行入力 */
	/* 行末の改行は削除. CC_obufは破壊     */
{
	char *p;

	p = Res_p;
	if (*p == 0)
		return NULL;
	Res_p = strchr(p, '\n');
	if (Res_p != NULL) {
		if (Res_p[-1] == '\r')
			Res_p[-1] = 0;
		*Res_p++ = 0;
	} else {
		Res_p = STREND(p);
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
	if (*p == '=') {
		int l;
		p++;
		l = strcspn(p,"\t\n ");
		if (l < 1 || l >= (sizeof CC_v[0])-1)
			goto ERR;
		memcpy(CC_v[m], p, l);
		CC_v[m][l+1] = 0;
		p += l+1;
		goto RET;

  ERR:
		printfE(".CFG ファイルで $Ｎ 指定でおかしいものがある : $%s\n",p0);

	} else if (*p++ == ':') {
		n = *p++;	if (n < '1' || n > '9') goto ERR2;
		n -= '0';
		if (*p++ != '{') goto ERR2;
		i = 0;
		do {
			int l;
			l = strcspn(p,"|}");
			if (l < 1 || l >= (sizeof CC_v[0])-1)
				goto ERR;
			if (i == CC_no[n]) {
				memcpy(CC_v[m], p, l);
				CC_v[m][l+1] = 0;
				p = strchr(p,'}'); if (p == NULL) goto ERR2;
				p++;
				goto RET;
			}
			i++;
			p += l + 1;
		} while (p[-1] == '|');
  ERR2:
		printfE(".CFG ファイルで $Ｎ=文字列指定 または $Ｎ:Ｍ{..}指定でおかしいものがある : $%s\n",p0);
	}
  RET:
	return p;
}

char *Res_GetFileNameStr(char *d, char *s)
	/* s より空白で区切られた単語(ファイル名)をname にコピーする. */
	/* ただし "file name" のように"があれば"を削除し替りに間の空白を残す */
{
	int f = 0;

	s = StrSkipSpc(s);
	while (*s) {
		if (*s == '"')
			f ^= 1;
		else if (f == 0 && (*(unsigned char *)s <= ' '))
			break;
		else
			*d++ = *s;
		s++;
	}
	*d = 0;
	return s;
}

void Res_GetFmts(void)
{
	#define ISSPC(c)	((unsigned char)c <= ' ')
	char name[FIL_NMSZ];
	char *p,*d,*q;
	int  mode;

	d = CC_fmtBuf;
	mode = 0;
	while ( (p = Res_GetLine()) != NULL ) {
		q = StrSkipSpc(p);
		if (strnicmp(q, "#begin", 6) == 0 && ISSPC(p[6])) {
			mode = 1;
			continue;
		} else if (strnicmp(q, "#body", 5) == 0 && ISSPC(p[5])) {
			mode = 0;
			continue;
		} else if (strnicmp(q, "#end", 4) == 0 && ISSPC(p[4])) {
			mode = 2;
			continue;
		}
		switch (mode) {
		case 0:	/* #body */
			while (p && *p) {
				p = StrSkipSpc(p);	/* 空白スキップ */
				switch (*p) {
				case '\0':
				case '#':
					goto NEXT_LINE;
				case '\'':
					if (p[1] == 0) {
						printfE("レスポンスファイル(定義ファイル中)の'変換文字列名'指定がおかしい\n");
					}
					p++;
					d = strchr(p, '\'');
					if (d) {
						*d = '\0';
						d = STPCPY(CC_fmtBuf, p);
					}
					break;
				case '=':
					/*memcpy(CC_fmtBuf, Res_p, strlen(Res_p)+1);*/
					d = CC_fmtBuf;
				  #if 0
					if (p[1]) {
						d = STPCPY(d, p+1);
						*d++ = '\n';
						*d   = '\0';
					}
				  #endif
					mode = 3;
					goto NEXT_LINE;
				case '-':				/* オプション文字列だ */
					q = StrSkipNotSpc(p);
					if (*q) {
						*q++ = 0;
					} else {
						q = NULL;
					}
					Opts(p);
					p = q;
					break;
				case '$':				/* $変数だ */
					p = Res_SetDoll(p+1);
					break;
				default:
					if (Opt_linInFlg) {	/* 行単位でファイル名を取得 */
						SLIST_Add(&fileListTop, p);
						goto NEXT_LINE;
					} else {			/* 空白区切りでファイル名を取得 */
						p = Res_GetFileNameStr(name, p);
						SLIST_Add(&fileListTop, name);
					}
				}
			}
		  NEXT_LINE:;
			break;
		case 1:	/* #begin */
			SLIST_Add(&beforeTextList, p);
			break;
		case 2:	/* #end  */
			SLIST_Add(&afterTextList, p);
			break;
		case 3: /* = バッファ溜め本体 */
			d = STPCPY(d, p);
			*d++ = '\n';
			*d   = '\0';
			break;
		}
	}
}

void GetResFile(char *name)
	/* レスポンスファイル入力 */
{
	FILE *fp;
	unsigned l;

	if (name[0] == 0) {						/* ファイル名がなければ標準入力 */
		l = fread(CC_obuf, 1, CC_FMTSIZ, stdin);
	} else {
		strcpy(Res_nm,name);
		FIL_AddExt(Res_nm, "ABX");
		fp = fopenE(Res_nm,"r");
		l = freadE(CC_obuf, 1, CC_FMTSIZ, fp);
		fclose(fp);
	}
	CC_obuf[l] = 0;
	if (l == 0)
		return;

	Res_p = CC_obuf;
	Res_GetFmts();	/* 実際のファイル内容の処理 */
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
						printfE("%s のある検索行に{..}が10個以上ある %s\n",
								Res_nm,lin);
					}
					memcpy(CC_v[CC_vn], f, l);
					CC_v[CC_vn++][l+1] = 0;
					k += l;
					f = strchr(f,'}');
					if (f == NULL) {
			  ERR1:
						printfE("%s で{..}の指定がおかしい %s\n",Res_nm, lin);
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
	/* 定義ファイル入力 */
{
	FILE *fp;
	unsigned l;
	char *p;

	FIL_FullPath(name, Res_nm);
	fp = fopenE(Res_nm,"r");
	l = freadE(CC_obuf, 1, CC_FMTSIZ, fp);
	fclose(fp);
	CC_obuf[l] = 0;
	if (l == 0)
		return;

	if (key[1] == 0) /* ':'だけの指定のとき */
		printf("':変換名'一覧\n");
	/*l = 1;*/
	/*   */
	strupr(key);
	Res_p = CC_obuf;
	/* 改行+':'+変換名を探す */
	while ((Res_p = strstr(Res_p, "\n:")) != NULL) {
		Res_p ++;
		p = Res_GetLine();
		if (p)
			p = strtok(p, " \t\r");
		/*printf("cmp %s vs %s\n",key,p);*/
		if (p == NULL || *p == 0)
			continue;
		strupr(p);
		if (key[1]) {
			/* 変換名が見つかればレスポンスと同じ処理をする */
			if (Res_StrCmp(key, p) == 0) {
				if ((p = strstr(Res_p, "\n:")) != NULL) {
					*p = '\0';
				}
				Res_GetFmts();
				return;
			}
		} else {	/* 検索キーがなければ、一覧表示 */
			printf("\t%s\n",p);
		  #if 0	/* 後回し .. */
			if (++l == 23) {
				printf("[more]");
				DOS_KbdIn();
				printf("\b\b\b\b\b\b      \b\b\b\b\b\b");
				l = 0;
			}
		  #endif
		}
	}
	if (key[1])
		printf("%s には %s は定義されていない\n", Res_nm, key);
	exit(1);
}


int main(int argc, char *argv[])
{
	int i,f;
	char *p;
	SLIST_T *sl;

	strcpy(exename, FIL_BaseName(argv[0]));		/*アプリケーション名*/
	strlwr(exename);

	if (argc < 2)
		Usage();

	CC_tmpDir[0] = 0;
	/*GetTmpDir(CC_tmpDir);*/

	FIL_ChgExt(strcpy(Opt_abxName,argv[0]), "CFG");
	for (i = 0; i < 10; i++)
		CC_v[i][0] = 0;
	CC_vn = 1;

	CC_fmtBuf[0] = 0;
	/* コマンドラインのオプション/ファイル名/変換文字列, 取得 */
	for (f = 0, i = 1; i < argc; i++) {
		p = argv[i];
	   LLL1:
		if (f) {
			if (CC_fmtBuf[0])
				strcat(CC_fmtBuf, " ");
			strcat(CC_fmtBuf, p);
			if (f == 1) {
				p = strrchr(CC_fmtBuf, '\'');
				if (p) {
					f = 0;
					*p = 0;
				}
			}
		} else if (*p == '\'') {
			f ^= 1;
			p++;
			if (*p)
				goto LLL1;

		} else if (*p == '=') {
			f = 2;
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
				printfE(":#で始まる文字列は指定できません（%s）\n",p);
			}
			GetCfgFile(Opt_abxName, p);
		} else if (*p == '$') {
			if (p[1] >= '1' && p[1] <= '9' && p[2] == '=') {
				strcpy(CC_v[p[1]-'0'], p+3);
			}
		} else {
			SLIST_Add(&fileListTop, p);
		}
	}

	if (CC_tmpDir[0] == 0)	/* テンポラリディレクトリ名取得 */
		FIL_GetTmpDir(CC_tmpDir);
	
	if (Opt_atr == 0) {		/* デフォルトのファイル検索属性 */
		Opt_atr = 0x127;
	}

	/* 変換文字列調整 */
	if (CC_fmtBuf[0] == '\0') {
		if (Opt_recFlg)
			strcpy(CC_fmtBuf, "$F\n");
		else
			strcpy(CC_fmtBuf, "$c\n");
	}
	p = strchr(CC_fmtBuf, '\n');
	if (p == NULL)
		strcat(CC_fmtBuf, "\n");

	/* 連番生成での初期値設定 */
	CC_num    = Opt_renbanStart;
	CC_numEnd = Opt_renbanEnd;

	/* バッチ実行のとき */
	if (Opt_batFlg) {
		strcpy(Opt_outname, CC_tmpDir);
		strcat(Opt_outname,"\\_abx_tmp.bat");
	}

	/* 出力ファイル設定 */
	if (Opt_outname[0]) {
		CC_fp = fopenE(Opt_outname, "w");
	} else {
		CC_fp = stdout;
	}

	if (Opt_batEx)					/* バッチ実行用に先頭に echo off を置く */
		fprintf(CC_fp, "@echo off\n");
	/* 直前出力テキスト */
	for (sl = beforeTextList; sl != NULL; sl = sl->link)
		CC_WriteLine0(sl->s);

	/* -u && -s ならば、指定ファイル名を大文字化 */
	if (CC_upLwrFlg && Opt_sort) {
		Opt_sort |= 0x8000;	/* ファイル検索して見つかったファイル名を大文字化する指定 */
		for (sl = fileListTop; sl != NULL; sl = sl->link) {
			strupr(sl->s);
		}
	}

	/* 実行 */
	if (CC_numEnd == 0) {
		for (sl = fileListTop; sl != NULL; sl = sl->link) {
			p = sl->s;
			CC_lineBuf = p;
			if (*p != '\\' && *p != '/' && p[1] != ':') {	/* 相対パスのとき */
				strcpy(Opt_iname, p);
				strcpy(Opt_abxName, Opt_ipath);
			} else {										/* フルパスのとき */
				char *s;
				strcpy(Opt_abxName, p);
				s = STREND(p);
				if (*s == '/' || *s == '\\')
					strcat(Opt_abxName, "*");
			}
			FIL_AddExt(Opt_abxName, Opt_dfltExtp);		/* デフォルト拡張子付加 */
			/* 実際のファイル名ごとの生成 */
			FSrh_FindAndDo(Opt_abxName, Opt_atr, Opt_recFlg,
				Opt_zenFlg, Opt_topN, Opt_sort, Opt_knjChk, Opt_nonFF,
				Opt_szmin, Opt_szmax, Opt_dtmin, Opt_dtmax, CC_Write);
		}
	} else {
		Opt_nonFF = 1;
		while (CC_num <= CC_numEnd) {
			sprintf(Opt_abxName, "%d", CC_num);
			CC_lineBuf = p = Opt_abxName;
			/* 実際のファイル名ごとの生成 */
			FSrh_FindAndDo(Opt_abxName, Opt_atr, Opt_recFlg,
				Opt_zenFlg, Opt_topN, Opt_sort, Opt_knjChk, Opt_nonFF,
				Opt_szmin, Opt_szmax, Opt_dtmin, Opt_dtmax, CC_Write);
		}
	}

	/* 直後出力テキスト */
	for (sl = afterTextList; sl != NULL; sl = sl->link)
		CC_WriteLine0(sl->s);
	if (Opt_batEx)	/* バッチ実行用にファイル末に:ENDを付加する */
		fprintf(CC_fp, ":END\n");

	if (Opt_outname[0])
		fclose(CC_fp);

	/* バッチ実行のとき */
	if (Opt_batFlg) {
		p = getenv("COMSPEC");
		/*i=*/ execl(p,p,"/c",Opt_outname,NULL);
		/* ※ dos(16)これ以降は実行されない... がwin95下ではどうかしらない */
	}

	return 0;
}
