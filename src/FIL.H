#ifndef __FIL_H__
#define __FIL_H__

#define	FIL_NMSZ	(260*8)

typedef struct find_t FIL_FIND;

char *FIL_BaseName(char *fname);
char * FIL_ChgExt(char *fname, char *ext);
char * FIL_AddExt(char *fname, char *ext);

#if 0
#define FIL_FindFirst(nm,ff,atr)	_dos_findfirst((nm),(ff),(atr))
#define FIL_FindNext(ff)			_dos_findnext(ff)
#else
#define FIL_FindFirst(nm,atr,ff)	findfirst((nm),(struct ffblk*)(ff),(atr))
#define FIL_FindNext(ff)			findnext((struct  ffblk*)(ff))
#endif

#define	FIL_FullPath(dst,src)		_fullpath((dst),(src),FIL_NMSZ)
#define FIL_MakePath(s,d,p,n,e)		_makepath(s,d,p,n,e)
#define FIL_SplitPath(s,d,p,n,e)	_splitpath(s,d,p,n,e)

void FIL_SetZenMode(int f);
int  FIL_GetZenMode(void);
void FIL_SetWccMode(int f);
int  FIL_GetWccMode(void);

/* ファイル属性
	0x01	取得専用
	0x02	不可視属性（通常、ディレクトリ検索で除外）
	0x04	システム・ファイル
	0x08	ボリューム名
	0x10	ディレクトリ
	0x20	保存ビット									*/


/*-------------------------------------------*/
/*-------------------------------------------*/
/*-------------------------------------------*/
/* めんどくさいので、ついでに、ここに書く^^; */
/*-------------------------------------------*/
int DOS_KbdIn(void);

#endif
