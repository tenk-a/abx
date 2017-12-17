/**
 *  @file   subr.cpp
 *  @brief  �T�u���[�`��
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @note
 *  	Boost Software License Version 1.0
 */
#include "subr.hpp"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
//#include <time.h>
#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif



/*---------------------------------------------------------------------------*/

char const* StrSkipSpc(char const* s)
{
    while (*s && *(unsigned char *)s <= ' ')
    	s++;
    return s;
}

char const* StrSkipNotSpc(char const* s)
{
    while (*(unsigned char *)s > ' ')
    	s++;
    return s;
}


char *StrLwrN(char* str, size_t size)
{
    unsigned char* s = (unsigned char*)str;
    unsigned char* e = s + size;
    while (s < e) {
    	unsigned c = *s;
    	if ('A' <= c && c <= 'Z') {
    	    c += 'a' - 'A';
    	    *s = (unsigned char)c;
    	}
    	++s;
    }
    return str;
}


char *FIL_DelLastDirSep(char *dir)
{
    if (dir) {
    	char *p = FIL_BaseName(dir);
    	if (strlen(p) > 1) {
    	    p = STREND(dir);
    	    if (p[-1] == '\\' || p[-1] == '/')
    	    	p[-1] = 0;
    	}
    }
    return dir;
}



/*---------------------------------------------------------------------------*/

/* �Ƃ肠�����A�A�Z���u���\�[�X�Ƃ̌��ˍ����ŁA�_�~�[�֐���p�� */
static int  FIL_zenFlg = 1; 	    /* 1:MS�S�p�ɑΉ� 0:���Ή� */


void	FIL_SetZenMode(int ff)
{
    FIL_zenFlg = ff;
}

int FIL_GetZenMode(void)
{
    return FIL_zenFlg;
}


char *FIL_BaseName(char const* adr)
{
    char const* p;

    p = adr;
    while (*p != '\0') {
    	if (*p == ':' || *p == '/' || *p == '\\')
    	    adr = p + 1;
    	if (FIL_zenFlg && ISKANJI((*(unsigned char *)p)) && *(p+1) )
    	    p++;
    	p++;
    }
    return (char*)adr;
}


char *FIL_ChgExt(char filename[], char const* ext)
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


char *FIL_AddExt(char filename[], char const* ext)
{
    if (ext) {
    	if (strrchr(FIL_BaseName(filename), '.') == NULL) {
    	    strcat(filename,".");
    	    strcat(filename, ext);
    	}
    }
    return filename;
}


char *FIL_NameUpr(char *s0)
{
    /* �S�p�Q�o�C�g�ڂ��l������ strupr */
    char *s = s0;
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
    return s0;
}


int FIL_FdateCmp(const char *tgt, const char *src)
{
    // ��̃t�@�C���̓��t�̑召���r����.
    // tgt ���V������� 1(��), �����Ȃ�� 0, tgt ���Â���� -1(��)
    struct _finddatai64_t srcData;
    struct _finddatai64_t tgtData;
    intptr_t srcFindHdl, tgtFindHdl;
    time_t srcTm, tgtTm;

    srcFindHdl = _findfirsti64((char *)src, &srcData);
    srcTm = (srcFindHdl == -1) ? 0 : srcData.time_write;

    tgtFindHdl = _findfirsti64((char *)tgt, &tgtData);
    if (tgtFindHdl == -1)
    	return -1;
    tgtTm = (tgtFindHdl == -1) ? 0 : tgtData.time_write;

    if (tgtTm < srcTm)
    	return -1;
    else if (tgtTm > srcTm)
    	return 1;
    return 0;
}


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
    	      #if 1
    	    	p = (char*)".\\";
    	      #else
    	    	printfE("���ϐ�TMP��TEMP, �܂���-w<DIR>�Ńe���|�����E�f�B���N�g�����w�肵�Ă�������\n");
    	    	/*printfE("���ϐ�TMP��TEMP�������إ�ިڸ�؂��w�肵�Ă�������\n");*/
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





//===========================================================================
// �e���|�����E�t�@�C��.
//===========================================================================

#ifdef _WIN32
#define FNAME_MAX   	(2U*1024)

static char* fname_cpy(char dst[], size_t l, char const* src)
{
    strncpy(dst, src, l);
    dst[l-1] = '\0';
    return dst;
}

/** ���ϐ�tmp �� temp ������΂��̓��e���A�Ȃ����"."�����ĕԂ�.
 *  @return 	0:tmp,temp����������.  1:������.
 */
int TmpFile_getTmpEnv(char tmpEnv[], size_t size)
{
    const char* p = getenv("TMP");
    int     	    	    f = 1;
    assert(tmpEnv && size > 0);
    if (p == NULL) {
    	p = getenv("TEMP");
    	if (p == NULL) {
    	    p = ".";
    	    f = 0;
    	}
    }
    fname_cpy(tmpEnv, size, p);
    return f;
}
#else
// �K���Ɍv�Z.
#define FNAME_MAX   	(6U*1024)

/** /tmp ��Ԃ�.
 *  @return 	1
 */
int TmpFile_getTmpEnv(char tmpEnv[], size_t size)
{
    fname_cpy(tmpEnv, size, "/tmp");
    return 1;
}
#endif



/** �e���|�����t�@�C�����쐬. ���������name��Ԃ��A���s����NULL.
 *  prefix,surffix �Ńt�@�C�����̗��[������ݒ�
 *  ��������ƁA�e���|�����f�B���N�g���ɂ��̖��O�̃t�@�C�����ł���.(close��)
 *  (�܂莩���ō폜���Ȃ��Ƒʖ�)
 */
char* TmpFile_make2(char name[], size_t size, const char* prefix, char const* suffix)
{
  #ifdef _WIN32
    char    tmpd[ FNAME_MAX + 2];
    if (!name || size < 20) {
    	assert(name != 0 && size >= 20);
    	return NULL;
    }
    if (!prefix)
    	prefix = "";
    if (!suffix)
    	suffix = "";
    if (strlen(prefix) + strlen(suffix) + 20 >= size)
    	return NULL;
    tmpd[0] = 0;
    TmpFile_getTmpEnv(tmpd, FNAME_MAX);
    //FIL_GetTmpDir(tmpd);
    //printf("dir=%s\n", tmpd);
    unsigned pid = GetCurrentProcessId();
    pid = ((pid / 29) * 11 + (pid % 37)*0x10003) ^ ( 0x00102100);
 #if 1
    uint64_t tmr;
    QueryPerformanceCounter((union _LARGE_INTEGER*)&tmr);
 #else
    time_t   tmr;
    time(&tmr);
 #endif
    tmr *= 16;
    unsigned idx = 0;
    HANDLE   h;
    do {
    	++idx;
    	unsigned ti = unsigned(tmr + idx);
    	snprintf(name, size-1, "%s\\%s%08x-%08x%s", tmpd, prefix, pid, ti, suffix);
    	name[size-1] = 0;
    	h = CreateFile(name, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    } while (h == INVALID_HANDLE_VALUE && idx > 0);
    if (h == INVALID_HANDLE_VALUE)
    	return NULL;
    CloseHandle(h);
    return name;
  #endif
}



#if 0
/** �e���|�����t�@�C���쐬. ���������handle��Ԃ�.
 */
#ifdef _WIN32
//static HANDLE TmpFile_open(char name[], size_t size, const char* prefix)
//{
//  if (TmpFile_openName(name, size, prefix)) {
//  	return CreateFile(name, GENERIC_WRITE, 0, CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY, 0);
//  }
//}
#else
static int    TmpFile_open(char name[], size_t size, const char* prefix)
{
    char    tmpName[ FNAME_MAX + 2 ];
    int     hdl;
    assert(name != 0 && size > 0 && prefix != 0);
    fname_cat(tmpName, FNAME_MAX, "/tmp/");
    fname_cat(tmpName, FNAME_MAX, prefix);
    fname_cat(tmpName, FNAME_MAX, ".XXXXXX");
    hdl = mkstemp( tmpName );
    if (hdl >= 0) {
    	if (strlen(tmpName) < size) {
    	    fname_cpy(name, size, tmpName);
    	} else {    // �t�@�C�����������ƕԂ��Ȃ��Ȃ玸�s����.
    	    close(hdl);
    	    remove(tmpName);
    	    hdl = -1;
    	}
    }
    return hdl;
}
#endif



/** �e���|�����t�@�C�����쐬. ���������name��Ԃ��A���s����NULL.
 *  prefix ��windows����3�o�C�g.
 *  ��������ƁA�e���|�����f�B���N�g���ɂ��̖��O�̃t�@�C�����ł��Ă���̂Œ���.(close��)
 *  (�܂莩���ō폜���Ȃ��Ƒʖ�)
 */
char* TmpFile_make(char name[], size_t size, const char* prefix)
{
  #ifdef _WIN32
    char    tmpd[ FNAME_MAX + 2];
    char    nm[ FNAME_MAX + 2 ];
    int     val;
    assert(name != 0 && size > 0 && prefix != 0);
    TmpFile_getTmpEnv(tmpd, FNAME_MAX);
    val = GetTempFileNameA( tmpd, prefix, 0, nm );
    if (val > 0) {
    	fname_cpy(name, size, nm);
    	return name;
    }
    return 0;
  #else
    int hdl = TmpFile_open(name, size, prefix);
    if (hdl >= 0) { // �n���h�����擾�ł��Ă�����Awin���Ɏ����邽�߁A�t�@�C����close���ċA��.
    	close(hdl);
    	return name;
    } else {
    	return 0;
    }
  #endif
}

#endif
