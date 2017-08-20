/**
 *  @file   subr.h
 *  @brief  サブルーチン
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @note
 *      Boost Software License Version 1.0
 */
#ifndef SUBR_H
#define SUBR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef unsigned char  UCHAR;
typedef signed   char  SCHAR;
typedef unsigned       UINT;
typedef unsigned short USHORT;
typedef unsigned long  ULONG;

#if 0
typedef unsigned       uint;
typedef unsigned char  uint8;
typedef signed   char  sint8;
typedef short          sint16;
typedef unsigned short uint16;
typedef long           sint32;
typedef unsigned long  uint32;
#endif


#define MEMBER_OFFSET(t,m)  ((long)&(((t*)0)->m))   /* 構造体メンバ名の、オフセット値を求める */

#define ISDIGIT(c)      (((unsigned)(c) - '0') < 10U)
#define ISLOWER(c)      (((unsigned)(c)-'a') < 26U)
#define TOUPPER(c)      (ISLOWER(c) ? (c) - 0x20 : (c) )
#define ISKANJI(c)      ((unsigned)((c)^0x20) - 0xA1 < 0x3C)
#define ISKANJI2(c)     ((UCHAR)(c) >= 0x40 && (UCHAR)(c) <= 0xfc && (c) != 0x7f)
#define STREND(p)       ((p)+strlen(p))
#define STPCPY(d,s)     (strcpy((d),(s))+strlen(s))     /* strlen(d)だと評価順によっては不味いのだった... */
#define STRINS(d,s)     (memmove((d)+strlen(s),(d),strlen(d)+1),memcpy((d),(s),strlen(s)))

#define MAX(x, y)       ((x) > (y) ? (x) : (y)) /* 最大値 */
#define MIN(x, y)       ((x) < (y) ? (x) : (y)) /* 最小値 */
#define ABS(x)          ((x) < 0 ? -(x) : (x))  /* 絶対値 */

#define REVW(a)         ((((a) >> 8) & 0xff)|(((a) & 0xff) << 8))
#define REVL(a)         ( (((a) & 0xff000000) >> 24)|(((a) & 0x00ff0000) >>  8)|(((a) & 0x0000ff00) <<  8)|(((a) & 0x000000ff) << 24) )

#define BB(a,b)         ((((unsigned char)(a))<<8)+(unsigned char)(b))
#define WW(a,b)         ((((unsigned short)(a))<<16)+(unsigned short)(b))
#define BBBB(a,b,c,d)   ((((unsigned char)(a))<<24)+(((unsigned char)(b))<<16)+(((unsigned char)(c))<<8)+((unsigned char)(d)))

#define GLB(a)          ((unsigned char)(a))
#define GHB(a)          GLB(((unsigned short)(a))>>8)
#define GLLB(a)         GLB(a)
#define GLHB(a)         GHB(a)
#define GHLB(a)         GLB(((unsigned long)(a))>>16)
#define GHHB(a)         GLB(((unsigned long)(a))>>24)
#define GLW(a)          ((unsigned short)(a))
#define GHW(a)          GLW(((unsigned long)(a))>>16)

#define PEEKB(a)        (*(unsigned char  *)(a))
#define PEEKW(a)        (*(unsigned short *)(a))
#define PEEKD(a)        (*(unsigned long  *)(a))
#define POKEB(a,b)      (*(unsigned char  *)(a) = (b))
#define POKEW(a,b)      (*(unsigned short *)(a) = (b))
#define POKED(a,b)      (*(unsigned long  *)(a) = (b))
#define PEEKiW(a)       ( PEEKB(a) | (PEEKB((unsigned long)(a)+1)<< 8) )
#define PEEKiD(a)       ( PEEKiW(a) | (PEEKiW((unsigned long)(a)+2) << 16) )
#define PEEKmW(a)       ( (PEEKB(a)<<8) | PEEKB((unsigned long)(a)+1) )
#define PEEKmD(a)       ( (PEEKmW(a)<<16) | PEEKmW((unsigned long)(a)+2) )
#define POKEmW(a,b)     (POKEB((a),GHB(b)), POKEB((ULONG)(a)+1,GLB(b)))
#define POKEmD(a,b)     (POKEmW((a),GHW(b)), POKEmW((ULONG)(a)+2,GLW(b)))
#define POKEiW(a,b)     (POKEB((a),GLB(b)), POKEB((ULONG)(a)+1,GHB(b)))
#define POKEiD(a,b)     (POKEiW((a),GLW(b)), POKEiW((ULONG)(a)+2,GHW(b)))

#define DB              if (debugflag)
extern int              debugflag;


/* memマクロ */
#define MEMCPY(d0,s0,c0) do {       \
    char *d__ = (void*)(d0);        \
    char *s__ = (void*)(s0);        \
    int c__ = (c0);                 \
    do {                            \
        *d__++ = *s__++;            \
    } while (--c__);                \
} while(0)

#define MEMCPY2(d0,s0,c0) do {      \
    short *d__ = (void*)(d0);       \
    short *s__ = (void*)(s0);       \
    int c__ = (c0)>>1;              \
    do {                            \
        *d__++ = *s__++;            \
    } while (--c__);                \
} while(0)

#define MEMCPY4(d0,s0,c0) do {      \
    long *d__ = (void*)(d0);        \
    long *s__ = (void*)(s0);        \
    int c__ = (unsigned)(c0)>>2;    \
    do {                            \
        *d__++ = *s__++;            \
    } while (--c__);                \
} while(0)

#define MEMCPYW(d0,s0,c0) do {      \
    short *d__ = (void*)(d0);       \
    short *s__ = (void*)(s0);       \
    int c__ = (c0);                 \
    do {                            \
        *d__++ = *s__++;            \
    } while (--c__);                \
} while(0)

#define MEMCPYD(d0,s0,c0) do {      \
    long *d__ = (void*)(d0);        \
    long *s__ = (void*)(s0);        \
    int c__ = (c0);                 \
    do {                            \
        *d__++ = *s__++;            \
    } while (--c__);                \
} while(0)

#define MEMSET(d0,s0,c0) do {       \
    char *d__ = (void*)(d0);        \
    int c__ = (c0);                 \
    do {                            \
        *d__++ = (char)(s0);        \
    } while(--c__);                 \
} while(0)

#define MEMSETW(d0,s0,c0) do {      \
    short *d__ = (void*)(d0);       \
    int c__ = (c0);                 \
    do {                            \
        *d__++ = (short)(s0);       \
    } while(--c__);                 \
} while(0)

#define MEMSETD(d0,s0,c0) do {      \
    long *d__ = (void*)(d0);        \
    int c__ = (c0);                 \
    do {                            \
        *d__++ = (long)(s0);        \
    } while(--c__);                 \
} while(0)



/*------------------------------------------*/
/*------------------------------------------*/
/*------------------------------------------*/
#define STDERR      stdout

char *strncpyZ(char *dst, char *src, size_t size);
char *StrSkipSpc(char *s);
char *StrSkipNotSpc(char *s);
int  FIL_GetTmpDir(char *t);
char *FIL_DelLastDirSep(char *dir);
void FIL_LoadE(char *name, void *buf, int size);
void FIL_SaveE(char *name, void *buf, int size);
void *FIL_LoadM(char *name);
void *FIL_LoadME(char *name);
extern int  FIL_loadsize;
volatile void printfE(char *fmt, ...);
void *mallocE(size_t a);
void *reallocE(void *a, size_t b);
void *callocE(size_t a, size_t b);
char *strdupE(char *p);
void freeE(void *p);
FILE *fopenE(char *name, char *mod);
size_t  fwriteE(void *buf, size_t sz, size_t num, FILE *fp);
size_t  freadE(void *buf, size_t sz, size_t num, FILE *fp);

typedef struct SLIST_T {
    struct SLIST_T  *link;
    char            *s;
} SLIST_T;
SLIST_T *SLIST_Add(SLIST_T **root, char *s);



#ifdef C16  /* 16ビット版のとき */

#define FIL_NMSZ    260

typedef struct FIL_FIND {
    char            reserved[21];
    char            attrib;
    unsigned short  wr_time;
    unsigned short  wr_date;
    long            size;
    char            name[14];
    char            srchname[14];
} FIL_FIND;

char far * (cdecl far FIL_BaseName)(char far *fname);
char far * (cdecl far FIL_ChgExt)(char far *fname, char far *ext);
char far * (cdecl far FIL_AddExt)(char far *fname, char far *ext);

int  cdecl far  FIL_FindFirst(char far *fname, unsigned atr, FIL_FIND far *ff);
int  cdecl far  FIL_FindNext(FIL_FIND far *ff);
#define FIL_FIND_HANDLE             int
#define FIL_FIND_HANDLE_OK(hdl)     ((hdl) == 0)
#define FIL_FINDFIRST(nm,atr,ff)    FIL_FindFirst(nm,atr,ff)
#define FIL_FINDNEXT(hdl,ff)        FIL_FindNext(ff)
#define FIL_FINDCLOSE(hdl)

char far * (cdecl far FIL_FullPath)(char far *src, char far *dst);
void cdecl far FIL_SetZenMode(int f);
int  cdecl far FIL_GetZenMode(void);
void cdecl far FIL_SetWccMode(int f);
int  cdecl far FIL_GetWccMode(void);

#ifdef __TURBOC__                   /* TC++ v1 以前対策? */
#define FIL_MakePath(s,d,p,n,e)     fnmerge(s,d,p,n,e)
#define FIL_SplitPath(s,d,p,n,e)    fnsplit(s,d,p,n,e)
#else
#define FIL_MakePath(s,d,p,n,e)     _makepath(s,d,p,n,e)
#define FIL_SplitPath(s,d,p,n,e)    _splitpath(s,d,p,n,e)
#endif

#else       /* 32ビット版のとき */

//#define FIL_NMSZ  (2052)      /* 1024 */
#define FIL_NMSZ    (0x4000)    /* 1024 */

char *FIL_BaseName(char *adr);
char *FIL_ChgExt(char filename[], char *ext);
char *FIL_AddExt(char filename[], char *ext);
char *FIL_NameUpr(char *s);
void FIL_SetZenMode(int f);
int  FIL_GetZenMode(void);
void FIL_SetWccMode(int f);
int  FIL_GetWccMode(void);

#ifdef __BORLANDC__
typedef struct find_t               FIL_FIND;
#define FIL_FIND_HANDLE             int
#define FIL_FIND_HANDLE_OK(hdl)     ((hdl) == 0)
#define FIL_FINDFIRST(nm,atr,ff)    findfirst((nm),(struct ffblk*)(ff),(atr))
#define FIL_FINDNEXT(hdl,ff)        findnext((struct  ffblk*)(ff))
#define FIL_FINDCLOSE(hdl)
#elif defined _MSC_VER
#include <io.h>
typedef struct _finddata_t          FIL_FIND;
#define FIL_FIND_HANDLE             intptr_t
#define FIL_FIND_HANDLE_OK(hdl)     ((hdl) >= 0)
#define FIL_FINDFIRST(nm,atr,ff)    (_findfirst((nm),(struct _finddata_t*)(ff)))
#define FIL_FINDNEXT(hdl,ff)        _findnext((hdl), (struct _finddata_t*)(ff))
#define FIL_FINDCLOSE(hdl)          _findclose(hdl)
#define wr_date                     time_write
#else   // win32 失敗中...
#include <windows.h>
typedef WIN32_FIND_DATA             FIL_FIND;
#define FIL_FIND_HANDLE             HANDLE
#define FIL_FIND_HANDLE_OK(hdl)     ((hdl) != INVALID_HANDLE_VALUE)
#define FIL_FINDFIRST(nm,atr,ff)    (FIL_Find_Handle= FindFirstFile((nm),(WIN32_FIND_DATA*)(ff)))
#define FIL_FINDNEXT(hdl,ff)        FindNextFile((hdl), (WIN32_FIND_DATA*)(ff))
#define FIL_FINDCLOSE(hdl)          FindClose(hdll)
#define wr_date                     ftLastWriteTime
#endif

#define FIL_FullPath(src,dst)       _fullpath((dst),(src),FIL_NMSZ)
#define FIL_MakePath(s,d,p,n,e)     _makepath(s,d,p,n,e)
#define FIL_SplitPath(s,d,p,n,e)    _splitpath(s,d,p,n,e)
int FIL_FdateCmp(const char *tgt, const char *src);

#endif

/* ファイル属性
    0x01    取得専用
    0x02    不可視属性（通常、ディレクトリ検索で除外）
    0x04    システム・ファイル
    0x08    ボリューム名
    0x10    ディレクトリ
    0x20    保存ビット                                  */


#endif  /* SUBR_H */
