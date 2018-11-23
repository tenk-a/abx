/**
 *  @file   subr.h
 *  @brief  �T�u���[�`��
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
#define STPCPY(d,s) 	(strcpy((d),(s))+strlen(s)) 	/* strlen(d)���ƕ]�����ɂ���Ă͕s�����̂�����... */
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
#else	// win32 ���s��...
#include <windows.h>
typedef WIN32_FIND_DATA     	    FIL_FIND;
#define FIL_FIND_HANDLE     	    HANDLE
#define FIL_FIND_HANDLE_OK(hdl)     ((hdl) != INVALID_HANDLE_VALUE)
#define FIL_FINDFIRST(nm,atr,ff)    (FIL_Find_Handle= FindFirstFile((nm),(WIN32_FIND_DATA*)(ff)))
#define FIL_FINDNEXT(hdl,ff)	    FindNextFile((hdl), (WIN32_FIND_DATA*)(ff))
#define FIL_FINDCLOSE(hdl)  	    FindClose(hdll)
#define wr_date     	    	    ftLastWriteTime
#endif

/* �t�@�C������
    0x01    �擾��p
    0x02    �s�������i�ʏ�A�f�B���N�g�������ŏ��O�j
    0x04    �V�X�e���E�t�@�C��
    0x08    �{�����[����
    0x10    �f�B���N�g��
    0x20    �ۑ��r�b�g	    	    	    	    	*/


#endif	/* SUBR_H */
