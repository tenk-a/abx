#include "subr.h"
#include <stdarg.h>
#include <io.h>



/* ------------------------------------------------------------------------ */
int		debugflag;

char *strncpyz(char *dst, char *src, size_t size)
{
	strncpy(dst, src, size);
	dst[size-1] = 0;
	return dst;
}

char *StrSkipSpc(char *s)
{
	while (*s && *(unsigned char *)s <= ' ')
		s++;
	return s;
}

char *StrSkipNotSpc(char *s)
{
	while (*(unsigned char *)s > ' ')
		s++;
	return s;
}

char *FIL_DelLastDirSep(char *dir)
{
	char *p;

	if (dir) {
		p = FIL_BaseName(dir);
		if (strlen(p) > 1) {
			p = STREND(dir);
			if (p[-1] == '\\' || p[-1] == '/')
				p[-1] = 0;
		}
	}
	return dir;
}



/*--------------------------------------*/

SLIST_T *SLIST_Add(SLIST_T **p0, char *s)
{
	SLIST_T *p;

	p = *p0;
	if (p == NULL) {
		p = callocE(1, sizeof(SLIST_T));
		p->s = strdupE(s);
		*p0 = p;
	} else {
		while (p->link != NULL) {
			p = p->link;
		}
		p->link = callocE(1, sizeof(SLIST_T));
		p = p->link;
		p->s = strdupE(s);
	}
	return p;
}



/*--------------------- エラー処理付きの標準関数 ---------------------------*/
volatile void printfE(char *fmt, ...)
{
	va_list app;

	va_start(app, fmt);
/*	fprintf(stdout, "%s %5d : ", src_name, src_line);*/
	vfprintf(stdout, fmt, app);
	va_end(app);
	exit(1);
}

void *mallocE(size_t a)
	/* エラーがあれば即exitの malloc() */
{
	void *p;
 #if 1
	if (a == 0)
		a = 1;
 #endif
	p = malloc(a);
//printf("malloc(0x%x)\n",a);
	if (p == NULL) {
		printfE("メモリが足りません( %d byte(s))\n",a);
	}
	return p;
}

void *callocE(size_t a, size_t b)
	/* エラーがあれば即exitの calloc() */
{
	void *p;

 #if 1
	if (b == 0)
		b = 1;
 #endif
	p = calloc(a,b);
//printf("calloc(0x%x,0x%x)\n",a,b);
	if (p == NULL) {
		printfE("メモリが足りません(%d*%d byte(s))\n",a,b);
	}
	return p;
}

void *reallocE(void *m, size_t a)
	/* エラーがあれば即exitの calloc() */
{
	void *p;
	p = realloc(m, a);
//printf("realloc(0x%x,0x%x)\n",m,a);
	if (p == NULL) {
		printfE("メモリが足りないです(%d byte(s))\n",a);
	}
	return p;
}

char *strdupE(char *s)
	/* エラーがあれば即exitの strdup() */
{
	char *p;
//printf("strdup('%s')\n",s);
	if (s == NULL)
		return callocE(1,1);
	p = strdup(s);
	if (p == NULL) {
		printfE("メモリが足りません(長さ%d+1)\n",strlen(s));
	}
	return p;
}

void freeE(void *p)
{
	if (p)
		free(p);
	/*return 0;*/
}

/* ------------------------------------------------------------------------ */
FILE *fopenE(char *name, char *mod)
	/* エラーがあれば即exitの fopen() */
{
	FILE *fp;
	fp = fopen(name,mod);
	if (fp == NULL) {
		printfE("ファイル %s をオープンできません\n",name);
	}
	setvbuf(fp, NULL, _IOFBF, 1024*1024);
	return fp;
}

size_t  fwriteE(void *buf, size_t sz, size_t num, FILE *fp)
	/* エラーがあれば即exitの fwrite() */
{
	size_t l;

	l = fwrite(buf, sz, num, fp);
	if (ferror(fp)) {
		printfE("ファイル書込みでエラー発生\n");
	}
	return l;
}

size_t  freadE(void *buf, size_t sz, size_t num, FILE *fp)
	/* エラーがあれば即exitの fread() */
{
	size_t l;

	l = fread(buf, sz, num, fp);
	if (ferror(fp)) {
		printfE("ファイル読込みでエラー発生\n");
	}
	return l;
}


/* ------------------------------------------------------------------------ */
int FIL_GetTmpDir(char *t)
{
	char *p;
	char nm[FIL_NMSZ+2];

	if (*t) {
		p = STPCPY(nm, t);
	} else {
		p = getenv("TMP");
		if (p == NULL) {
			p = getenv("TEMP");
			if (p == NULL) {
			  #if 10
				p = ".\\";
			  #else
				printfE("環境変数TMPかTEMP, または-w<DIR>でテンポラリ・ディレクトリを指定してください\n");
				/*printfE("環境変数TMPかTEMPでﾃﾝﾎﾟﾗﾘ･ﾃﾞｨﾚｸﾄﾘを指定してください\n");*/
			  #endif
			}
		}
		p = STPCPY(nm, p);
	}
	if (p[-1] != '\\' && p[-1] != ':' && p[-1] != '/')
		strcat(nm,"\\");
	strcat(nm,"*.*");
	_fullpath(t, nm, FIL_NMSZ);
	p = FIL_BaseName(t);
	*p = 0;
	if (p[-1] == '\\')
		p[-1] = 0;
	return 0;
}


/*---------------------------------------------------------------------------*/
/* 32ビット版のとき ... 16ビット版はアセンブラソースのほう */
#ifndef C16

/* とりあえず、アセンブラソースとの兼ね合いで、ダミー関数を用意 */
static int	FIL_zenFlg = 1;			/* 1:MS全角に対応 0:未対応 */
static int	FIL_wcFlg  = 0x08;


void	FIL_SetZenMode(int ff)
{
	FIL_zenFlg = ff;
}

int	FIL_GetZenMode(void)
{
	return FIL_zenFlg;
}


void	FIL_SetWcMode(int ff)
{
	FIL_wcFlg = ff;
}

int	FIL_GetWcMode(void)
{
	return FIL_wcFlg;
}


/*--------------------------------------------*/

char *FIL_BaseName(char *adr)
{
	char *p;

	p = adr;
	while (*p != '\0') {
		if (*p == ':' || *p == '/' || *p == '\\')
			adr = p + 1;
		if (FIL_zenFlg && ISKANJI((*(unsigned char *)p)) && *(p+1) )
			p++;
		p++;
	}
	return adr;
}

char *FIL_ChgExt(char filename[], char *ext)
{
	char *p;

	p = FIL_BaseName(filename);
	p = strrchr( p, '.');
	if (p == NULL) {
		if (ext) {
			strcat(filename,".");
			strcat( filename, ext);
		}
	} else {
		if (ext == NULL)
			*p = 0;
		else
			strcpy(p+1, ext);
	}
	return filename;
}


char *FIL_AddExt(char filename[], char *ext)
{
	if (ext) {
		if (strrchr(FIL_BaseName(filename), '.') == NULL) {
			strcat(filename,".");
			strcat(filename, ext);
		}
	}
	return filename;
}


char *FIL_NameUpr(char *s)
{
	/* 全角２バイト目を考慮した strupr */
	while (*s) {
		if (FIL_zenFlg && ISKANJI(*s) && s[1]) {
			s += 2;
		} else if (ISLOWER(*s)) {
			*s = TOUPPER(*s);
			s++;
		} else {
			s++;
		}
	}
}

#endif

/*---------------------------------------------------------------------------*/

