#ifndef __FIL_H__
#define __FIL_H__

#define	FIL_NMSZ	(260)
typedef struct FIL_FIND {
    char			reserved[21];
    char			attrib;
    unsigned short	wr_time;
    unsigned short	wr_date;
    long			size;
    char			name[14];
	char			srchname[14];
} FIL_FIND;

char far * (far FIL_BaseName)(char far *fname);
char far * (far FIL_ChgExt)(char far *fname, char far *ext);
char far * (far FIL_AddExt)(char far *fname, char far *ext);
int  far FIL_FindFirst(char far *fname, unsigned atr, FIL_FIND far *ff);
int  far FIL_FindNext(FIL_FIND far *ff);
char far * (far FIL_FullPath)(char far *src, char far *dst);
void far FIL_SetZenMode(int f);
int  far FIL_GetZenMode(void);
void far FIL_SetWccMode(int f);
int  far FIL_GetWccMode(void);

#ifdef __TURBOC__
#define FIL_MakePath(s,d,p,n,e)		fnmerge(s,d,p,n,e)
#define FIL_SplitPath(s,d,p,n,e)	fnsplit(s,d,p,n,e)
#else
#define FIL_MakePath(s,d,p,n,e)		_makepath(s,d,p,n,e)
#define FIL_SplitPath(s,d,p,n,e)	_splitpath(s,d,p,n,e)
#endif


/* めんどくさいので、ついでに、ここに書く^^; */
int far DOS_KbdIn(void);

#endif


// ファイル属性
//	0x01	取得専用
//	0x02	不可視属性（通常、ディレクトリ検索で除外）
//	0x04	システム・ファイル
//	0x08	ボリューム名
//	0x10	ディレクトリ
//	0x20	保存ビット
