/**
 *  @file   subr.h
 *  @brief  サブルーチン
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @note
 *  	Boost Software License Version 1.0
 */
#ifndef SUBR_H
#define SUBR_H

#include <stddef.h>
#include <stdio.h>

#define ISKANJI(c)      (((unsigned char)(c) >= 0x81 && (unsigned char)(c) <= 0x9F) || ((unsigned char)(c) >= 0xE0 && (unsigned char)(c) <= 0xFC))	//#define ISKANJI(c)  	((unsigned short)((c)^0x20) - 0xA1 < 0x3C)
#define STREND(p)   	((p)+strlen(p))
#define STPCPY(d,s) 	(strcpy((d),(s))+strlen(s)) 	/* strlen(d)だと評価順によっては不味いのだった... */
//#define STRINS(d,s)	(memmove((d)+strlen(s),(d),strlen(d)+1),memcpy((d),(s),strlen(s)))

char const* StrSkipSpc(char const* s);
char const* StrSkipNotSpc(char const* s);

//enum { FIL_NMSZ = 2052 };
enum { FIL_NMSZ = 1024+16 };

void FIL_SetZenMode(int f);
int  FIL_GetZenMode(void);

#if defined _MSC_VER
#include <io.h>
typedef struct _finddata_t  	    FIL_FIND;
//typedef struct _finddata32_t  	FIL_FIND;
#define FIL_FIND_HANDLE     	    intptr_t
#define FIL_FIND_HANDLE_OK(hdl)     ((hdl) >= 0)
#define FIL_FINDFIRST(nm,atr,ff)    (_findfirst((nm),(struct _finddata_t*)(ff)))
#define FIL_FINDNEXT(hdl,ff)	    _findnext((hdl), (struct _finddata_t*)(ff))
#define FIL_FINDCLOSE(hdl)  	    _findclose(hdl)
#define wr_date     	    	    time_write
#else	// win32 失敗中...
#include <windows.h>
typedef WIN32_FIND_DATA     	    FIL_FIND;
#define FIL_FIND_HANDLE     	    HANDLE
#define FIL_FIND_HANDLE_OK(hdl)     ((hdl) != INVALID_HANDLE_VALUE)
#define FIL_FINDFIRST(nm,atr,ff)    (FIL_Find_Handle= FindFirstFile((nm),(WIN32_FIND_DATA*)(ff)))
#define FIL_FINDNEXT(hdl,ff)	    FindNextFile((hdl), (WIN32_FIND_DATA*)(ff))
#define FIL_FINDCLOSE(hdl)  	    FindClose(hdll)
#define wr_date     	    	    ftLastWriteTime
#endif

/* ファイル属性
    0x01    取得専用
    0x02    不可視属性（通常、ディレクトリ検索で除外）
    0x04    システム・ファイル
    0x08    ボリューム名
    0x10    ディレクトリ
    0x20    保存ビット	    	    	    	    	*/


#endif	/* SUBR_H */
