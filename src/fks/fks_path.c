/**
 *	@file	fks_priv_fname_hdr.h
 *	@brief	�t�@�C���������֌W
 *	@author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 *	@note
 *	-	c/c++ �p.
 *	-	win / linux(unix) �p.
 *		win�ł�\ �� /���A�ȊO��/���Z�p���[�^.
 *	-	������0x80������ascii�n�ł��邱�ƑO��.
 *  -   sjis���̕����R�[�h����0x5c(\)��啶���������̂��Ƃ����邽�ߕ����R�[�h�z��.
 *		win: char �� dbc, wchar_t �� utf16 �z��. 
 *          FKS_PATH_UTF8 ���`����� char �� utf8.
 *			�t�@�C�����̑召�������ꎋ��ascii�͈͊O�ɂ��y�Ԃ̂�
 *			�����R�[�h����⏬�������͋ɗ� win api ��p����.
 *      ��os: utf8.  FKS_PATH_DBC ���`�����sjis,big5,gbk�l��(���ϐ�LANG�Q��),
 *			FKS_PATH_ASCII���`����� ���o�C�g�������l�����Ȃ�.
 *         	wchar_t �͂��Ԃ�utf32.
 *	- ��r�֌W�́A���P�[���Ή��s�\��
 */

#include <fks/fks_config.h>
#include <fks/fks_path.h>
#include <fks/fks_assert_ex.h>
#include <fks/fks_alloca.h>

//#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// os �̈Ⴂ�֌W.
#if defined FKS_USE_WIN_API
 #include <windows.h>
 #include <shlwapi.h>
 #if defined _MSC_VER
  #pragma comment(lib, "User32.lib")			// CharNext()
  #pragma comment(lib, "Shlwapi.lib")			// StrCmpNI()
 #endif
#endif

#if (defined __cplusplus) && !(defined FKS_PATH_WCS_COMPILE)
extern "C" {
#endif

// fullpath ����alloca���g���ꍇ�ꍇ��`.
#define FKS_PATH_USE_ALLOCA

// c/c++ �΍�.
#ifdef __cplusplus										// c++�̏ꍇ�A�|�C���^����݂̂̊֐���const��,��const�ł�����.
  #define FKS_PATH_const_CHAR		FKS_PATH_CHAR		// ���̂��߁A��{�́A��const�֐��ɂ���.
#else													// c�̏ꍇ�͕W�����C�u�����ɂ��킹 ����const�Ŗ߂�l ��const �ɂ���.
 #define FKS_PATH_const_CHAR		const FKS_PATH_CHAR
#endif

// FKS_PATH_UTF8,FKS_PATH_DBC,FKS_PATH_ASCII �̂����ꂩ���`
#if defined FKS_WIN32 // || defined FKS_MAC
 #define FKS_PATH_IGNORECASE
 #if !defined FKS_PATH_UTF8 && !defined FKS_PATH_DBC
  #define FKS_PATH_DBC
 #endif
#else
 #if !defined FKS_PATH_ASCII && !defined FKS_PATH_DBC && !defined FKS_PATH_UTF8
  #define FKS_PATH_UTF8
 #endif
#endif
#if defined FKS_PATH_UTF8 && defined FKS_PATH_DBC
 #error Both FKS_PATH_UTF8 and FKS_PATH_DBC are not defined
#endif


// ----------------------------------------------------------------------------
// macro

#define FKS_PATH_IS_DIGIT(c)		(('0') <= (c) && (c) <= ('9'))

// unicode�Ή�. �� mb�n���g��ꂽ���Ȃ����� tchar.h ���g�킸���O�őΏ�.
#ifdef FKS_PATH_WCS_COMPILE	// wchar_t �Ή�.
 #undef FKS_PATH_UTF8
 #undef FKS_PATH_DBC
 #undef FKS_PATH_ASCII
 #define FKS_PATH_C(x) 				L##x
 #define FKS_PATH_CHAR 				wchar_t
 #define FKS_PATH_R_STR(s,c)		wcsrchr((s),(c))		// '.'�����p
 #define FKS_PATH_STRTOLL(s,t,r)	wcstoll((s),(t),(r))
 #define FKS_PATH_ADJUSTSIZE(p,l)	(l)

 #ifdef FKS_USE_WIN_API
  #define FKS_PATH_CHARNEXT(p) 		(FKS_PATH_CHAR*)CharNextW((FKS_PATH_CHAR*)(p))
  #define FKS_STRLWR_N(s, n)		(CharLowerBuffW((s), (n)), (s))
  #define FKS_STRUPR_N(s, n)		(CharUpperBuffW((s), (n)), (s))
  #define FKS_STRLWR(s)				CharLowerW(s)
  #define FKS_STRUPR(s)				CharUpperW(s)
  #define FKS_PATH_TO_LOWER(c)		(wchar_t)CharLowerW((wchar_t*)(c))
  #define FKS_STR_N_CMP(l,r,n)		StrCmpNIW((l),(r),(n))
 #else
  #define FKS_PATH_CHARNEXT(p) 		((p) + 1)
  #define FKS_PATH_TO_LOWER(c)		(((c) >= FKS_PATH_C('A') && (c) <= FKS_PATH_C('Z')) ? (c) - FKS_PATH_C('A') + FKS_PATH_C('a') : (c))
  #if defined FKS_PATH_IGNORECASE	// �召�������ꎋ.
   #define FKS_STR_N_CMP(l,r,n)		wcsncasecmp((l),(r),(n))
  #else								// �召���.
   #define FKS_STR_N_CMP(l,r,n)		wcsncmp((l),(r),(n))
  #endif
 #endif
 #if defined FKS_PATH_IGNORECASE
  #define FKS_PATH_GET_C(c, p) 		((c) = *((p)++), (c) = FKS_PATH_TO_LOWER(c))
 #else
  #define FKS_PATH_GET_C(c, p) 		((c) = *((p)++))
 #endif
#else			// char �x�[�X.
 #define FKS_PATH_C(x) 				x
 #define FKS_PATH_CHAR 				char
 #define FKS_PATH_R_STR(s,c)		strrchr((s),(c))		// '.'�����v
 #define FKS_PATH_STRTOLL(s,t,r)	strtoll((s),(t),(r))

 #if defined FKS_PATH_UTF8
  #define FKS_PATH_ISMBBLEAD(c)		((unsigned)(c) >= 0x80)
  #if defined FKS_WIN32
   #define FKS_PATH_TO_LOWER(c)		((c < 0x10000) ? (wchar_t)CharLowerW((wchar_t*)(uint16_t)(c)) : (c))
  #else
   #define FKS_PATH_TO_LOWER(c)		(((c) >= FKS_PATH_C('A') && (c) <= FKS_PATH_C('Z')) ? (c) - FKS_PATH_C('A') + FKS_PATH_C('a') : (c))
  #endif
  #define FKS_PATH_CHARNEXT(p) 		fks_pathUtf8CharNext(p)
  #define FKS_PATH_ADJUSTSIZE(p,l)	fks_pathAdjustSize(p,l)
  #if defined FKS_PATH_IGNORECASE
   #define FKS_PATH_GET_C(c, p) 	(((c) = fks_pathUtf8GetC(&(p))), (c) = FKS_PATH_TO_LOWER(c))
  #else
   #define FKS_PATH_GET_C(c, p) 	((c) = fks_pathUtf8GetC(&(p)))
  #endif
 #elif defined FKS_WIN32
  #define FKS_PATH_ISMBBLEAD(c) 	IsDBCSLeadByte(c)
  #define FKS_STR_N_CMP(l,r,n)		StrCmpNIA((l),(r),(n))
  #define FKS_STRLWR_N(s,n)			(CharLowerBuffA((s),(n)), (s))
  #define FKS_STRUPR_N(s,n)			(CharUpperBuffA((s),(n)), (s))
  #define FKS_STRLWR(s)				CharLowerA(s)
  #define FKS_STRUPR(s)				CharUpperA(s)
  #define FKS_PATH_CHARNEXT(p) 		(FKS_PATH_CHAR*)CharNextA((FKS_PATH_CHAR*)(p))
  #define FKS_PATH_ADJUSTSIZE(p,l)	fks_pathAdjustSize(p,l)
  #define FKS_PATH_TO_LOWER(c)		(((c) >= FKS_PATH_C('A') && (c) <= FKS_PATH_C('Z')) ? (c) - FKS_PATH_C('A') + FKS_PATH_C('a') : (c))
 #else
  #if defined FKS_USE_FNAME_MBC
   #define FKS_PATH_ISMBBLEAD(c) 	((unsigned)(c) >= 0x80 && fks_pathIsZenkaku1(c) > 0)
   #define FKS_PATH_CHARNEXT(p) 	((p) + 1 + (FKS_PATH_ISMBBLEAD(*(unsigned char*)(p)) && (p)[1]))
  #define FKS_PATH_ADJUSTSIZE(p,l)	fks_pathAdjustSize(p,l)
  #else
   #define FKS_PATH_ISMBBLEAD(c) 	(0)
   #define FKS_PATH_CHARNEXT(p) 	((p) + 1)
  #define FKS_PATH_ADJUSTSIZE(p,l)	(l)
  #endif
  #if defined FKS_PATH_IGNORECASE	// �召�������ꎋ.
   #define FKS_STR_N_CMP(l,r,n)		strncasecmp((l),(r),(n))
  #else								// �召���.
   #define FKS_STR_N_CMP(l,r,n)		strncmp((l),(r),(n))
  #endif
  #define FKS_PATH_TO_LOWER(c)		(((c) >= FKS_PATH_C('A') && (c) <= FKS_PATH_C('Z')) ? (c) - FKS_PATH_C('A') + FKS_PATH_C('a') : (c))
 #endif
 #if !defined FKS_PATH_GET_C
  #if defined FKS_USE_FNAME_MBC
   #if !defined FKS_PATH_IGNORECASE
    #define FKS_PATH_GET_C(c, p) do {						\
		(c) = *(unsigned char*)((p)++); 					\
		if (FKS_PATH_ISMBBLEAD(c) && *(p)) 					\
			(c) = ((c) << 8) | *(unsigned char*)((p)++);	\
		else												\
			(c) = FKS_PATH_TO_LOWER(c);						\
	} while (0)
   #else
    #define FKS_PATH_GET_C(c, p) do {						\
		(c) = *(unsigned char*)((p)++); 					\
		if (FKS_PATH_ISMBBLEAD(c) && *(p)) 					\
			(c) = ((c) << 8) | *(unsigned char*)((p)++);	\
	} while (0)
   #endif
  #else
   #if defined FKS_PATH_IGNORECASE
    #define FKS_PATH_GET_C(c, p) 	((c) = *((p)++), (c) = FKS_PATH_TO_LOWER(c))
   #else
    #define FKS_PATH_GET_C(c, p) 	((c) = *((p)++))
   #endif
  #endif
 #endif
#endif

// �t�@�C������r�p�ɁA������̃|�C���^p����1�����擾����c�ɂ����}�N��.
// os��win�Ȃ�2�o�C�g�����Ή��ŏ�������. utf8�͔j�].


// ----------------------------------------------------------------------------
// UTF-8
#if defined FKS_PATH_UTF8
/** 1�����o�����|�C���^�X�V.
 */
static uint32_t	fks_pathUtf8GetC(const char** pStr) FKS_NOEXCEPT {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;

    if (c < 0x80) {
        ;
    } else if (*s) {
        int c2 = *s++;
        c2 &= 0x3F;
        if (c < 0xE0) {
            c = ((c & 0x1F) << 6) | c2;
        } else if (*s) {
            int c3 = *s++;
            c3 &= 0x3F;
            if (c < 0xF0) {
                c = ((c & 0xF) << 12) | (c2 << 6) | c3;
            } else if (*s) {
                int c4 = *s++;
                c4 &= 0x3F;
                if (c < 0xF8) {
                    c = ((c&7)<<18) | (c2<<12) | (c3<<6) | c4;
                } else if (*s) {
                    int c5 = *s++;
                    c5 &= 0x3F;
                    if (c < 0xFC) {
                        c = ((c&3)<<24) | (c2<<18) | (c3<<12) | (c4<<6) | c5;
                    } else if (*s) {
                        int c6 = *s++;
                        c6 &= 0x3F;
                        c = ((c&1)<<30) |(c2<<24) | (c3<<18) | (c4<<12) | (c5<<6) | c6;
                    }
                }
            }
        }
    }

    *pStr = (const char*)s;
    return c;
}


static char const* fks_pathUtf8CharNext(char const* pChr) {
    const unsigned char* s = (unsigned char*)pChr;
    unsigned c;
	if (!*s)			return (char const*)s;
    if (*s++ < 0x80)	return (char const*)s;
	if (!*s)			return (char const*)s;
    if (*s++ < 0xE0)	return (char const*)s;
	if (!*s)			return (char const*)s;
    if (*s++ < 0xF0)	return (char const*)s;
	if (!*s)			return (char const*)s;
    if (*s++ < 0xF8)	return (char const*)s;
	if (!*s)			return (char const*)s;
    if (*s++ < 0xFC)	return (char const*)s;
	if (!*s)			return (char const*)s;
	return (char const*)(s+1);
}

#endif


// ----------------------------------------------------------------------------
// win �ȊO�ŁADouble Byte Char�ɑΉ�����ꍇ�̏���.(�ȈՔ�)

#if defined FKS_PATH_DBC && !defined FKS_WIN32

static int	  s_fks_path_shift_char_mode =	0;

/** �Ƃ肠�����A0x5c�֌W�̑Ώ��p.
 */
FKS_STATIC_DECL(int)	fks_pathMbcInit(void) FKS_NOEXCEPT 
{
	const char* 	lang = getenv("LANG");
	const char* 	p;
	if (lang == 0)
		return -1;
	//s_fks_path_locale_ctype = strdup(lang);
	// ja_JP.SJIS �̂悤�Ȍ`���ł��邱�Ƃ�O���SJIS,big5,gbk�����`�F�b�N.
	p = strrchr(lang, '.');
	if (p) {
		const char* enc = p + 1;
		// 0x5c�΍􂪕K�v��encoding�����`�F�b�N.
		if (strncasecmp(enc, "sjis", 4) == 0) {
			return 1;
		} else if (strncasecmp(enc, "gbk", 3) == 0 || strncasecmp(enc, "gb2312", 6) == 0) {
			return 2;
		} else if (strncasecmp(enc, "big5", 4) == 0) {
			return 3;
		}
	}
	return -1;
}

FKS_STATIC_DECL(int)	fks_pathIsZenkaku1(unsigned c) FKS_NOEXCEPT
{
	if (s_fks_path_shift_char_mode	== 0)
		s_fks_path_shift_char_mode	= fks_pathMbcInit();
	switch (s_fks_path_shift_char_mode) {
	case 1 /* sjis */: return ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC));
	case 2 /* GBK  */: return  (c >= 0x81 && c <= 0xFE);
	case 3 /* BIG5 */: return ((c >= 0xA1 && c <= 0xC6) || (c >= 0xC9 && c <= 0xF9));
	default:		   return -1;
	}
}
#endif


// ----------------------------------------------------------------------------


/** �t�@�C���p�X�����̃f�B���N�g�����������t�@�C�����̈ʒu��Ԃ�.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathBaseName(FKS_PATH_const_CHAR *adr) FKS_NOEXCEPT
{
	const FKS_PATH_CHAR *p = adr;
	FKS_ARG_PTR_ASSERT(1, adr);
	while (*p) {
		if (*p == FKS_PATH_C(':') || fks_pathIsSep(*p))
			adr = (FKS_PATH_CHAR*)p + 1;
		p = FKS_PATH_CHARNEXT(p);
	}
	return (FKS_PATH_CHAR*)adr;
}


/** size�Ɏ��܂镶����̕�������Ԃ�. \0���܂܂Ȃ�.
 *	(win���ł͂Ȃ�ׂ��}���`�o�C�g�����̓r���ŏI���Ȃ��悤�ɂ���.
 *	 ���ǁA�p�r�I�ɂ͐؂��ȏ゠�܂�Ӗ��Ȃ�...)
 */
FKS_LIB_DECL (FKS_PATH_SIZE)
fks_pathAdjustSize(const FKS_PATH_CHAR* str, FKS_PATH_SIZE size) FKS_NOEXCEPT
{
 #if defined FKS_PATH_UTF8 || defined FKS_PATH_DBC
	const FKS_PATH_CHAR* s = str;
	const FKS_PATH_CHAR* b = s;
	const FKS_PATH_CHAR* e = s + size;
	FKS_ARG_PTR_ASSERT(1, str);
	FKS_ARG_ASSERT(1, (size > 0));
	while (s < e) {
		if (*s == 0)
			return s - str;
		b = s;
		s = FKS_PATH_CHARNEXT((FKS_PATH_CHAR*)s);
	}
	if (s > e)
		s = b;
	return s - str;
 #else
	return size;
 #endif
}


/** �t�@�C�����̃R�s�[. mbc�̎��͕��������Ȃ������܂�. dst == src ��ok.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathCpy(FKS_PATH_CHAR dst[], FKS_PATH_SIZE dstSz, const FKS_PATH_CHAR* src) FKS_NOEXCEPT
{
	FKS_PATH_SIZE	l;
	FKS_ARG_PTR_ASSERT(1, dst);
	FKS_ARG_ASSERT(2, (dstSz > 0));
	FKS_ARG_PTR0_ASSERT(3, src);

	if (src == NULL)
		return NULL;
	l = FKS_PATH_ADJUSTSIZE(src, dstSz);

	// �A�h���X�������Ȃ�A���������킹��̂�.
	if (dst == src) {
		dst[l] = 0;
		return dst;
	}

	// �R�s�[.
	{
		const FKS_PATH_CHAR*	s = src;
		const FKS_PATH_CHAR*	e = s + l;
		FKS_PATH_CHAR* 		d = dst;
		while (s < e)
			*d++ = *s++;
		*d = 0;
	}

	return dst;
}


/** �t�@�C����������̘A��.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathCat(FKS_PATH_CHAR dst[], FKS_PATH_SIZE dstSz, const FKS_PATH_CHAR* src) FKS_NOEXCEPT
{
	FKS_PATH_SIZE l;
	FKS_ARG_PTR_ASSERT(1, dst);
	FKS_ARG_ASSERT(2, (dstSz > 0));
	FKS_ARG_PTR0_ASSERT(3, src);
	if (src == 0)
		return NULL;
	FKS_ASSERT(src != 0 && dst != src);
	l = fks_pathLen(dst);
	if (l >= dstSz)		// ���������]���悪���t�Ȃ�T�C�Y�����̂�.
		return fks_pathCpy(dst, dstSz, dst);
	fks_pathCpy(dst+l, dstSz - l,	src);
	return dst;
}


/** �f�B���N�g�����ƃt�@�C��������������. fks_pathCat �ƈႢ�A\	/ ���Ԃɕt��.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathJoin(FKS_PATH_CHAR buf[], FKS_PATH_SIZE bufSz, const FKS_PATH_CHAR *dir, const FKS_PATH_CHAR *name) FKS_NOEXCEPT
{
	fks_pathCpy(buf, bufSz, dir);
	if (buf[0])
		fks_pathAddSep(buf, bufSz);
	fks_pathCat(buf, bufSz, name);
	return buf;
}


/** �t�@�C�����̑召��r.
 *	win/dos�n�͑召���ꎋ. �f�B���N�g���Z�p���[�^ \ / �����ꎋ.
 *	�ȊO�͒P���ɕ������r.
 */
FKS_LIB_DECL (int)
fks_pathCmp(const FKS_PATH_CHAR* l,	const FKS_PATH_CHAR* r) FKS_NOEXCEPT
{
	return fks_pathNCmp(l, r, (FKS_PATH_SIZE)-1);
}


/** �t�@�C�����̑召��r.
 *	win/dos�n�͑召���ꎋ. �f�B���N�g���Z�p���[�^ \ / �����ꎋ.
 *   * UNICODE�n�ł� ascii �ȊO�̑啶���������̑Ή��ł��邪�A
 *     ����mbc��ascii�݂̂Ŏ��ۂ̃t�@�C���V�X�e���Ƃ͈قȂ錋�ʂɂȂ�̂Œ���
 *	�����͒P���ɕ������r.(mac�͖��l��)
 */
FKS_LIB_DECL (int)
fks_pathNCmp(const FKS_PATH_CHAR* l, const FKS_PATH_CHAR* r, FKS_PATH_SIZE len) FKS_NOEXCEPT
{
 #ifdef FKS_STR_N_CMP
  #if 1
	FKS_ASSERT( l != 0 && r != 0 );
	return FKS_STR_N_CMP(l, r, len);
  #else
	int i;
	FKS_ASSERT( l != 0 && r != 0 );
	char*	orig = setlocale(LC_CTYPE, s_fks_path_locale_ctype);
	i = FKS_PATH_CMP(l, r);
	setlocale(orig);
	return i;
  #endif
 #else
	const FKS_PATH_CHAR* e = l + len;
	FKS_ASSERT( l != 0 && r != 0 );
	if (e < l)
		e = (const FKS_PATH_CHAR*)-1;
	while (l < e) {
		int 	 n;
		unsigned lc;
		unsigned rc;

		FKS_PATH_GET_C(lc, l);
		FKS_PATH_GET_C(rc, r);

		n  = (int)(lc - rc);
		if (n == 0) {
			if (lc == 0)
				return 0;
			continue;
		}
	 #if defined FKS_WINDOS
		if ((lc == FKS_PATH_C('/') && rc == FKS_PATH_C('\\')) || (lc == FKS_PATH_C('\\') && rc == FKS_PATH_C('/')))
			continue;
	 #endif
		return n;
	}
	return 0;
 #endif
}


FKS_STATIC_DECL (int) fks_pathNDigitCmp(const FKS_PATH_CHAR* l, const FKS_PATH_CHAR* r, FKS_PATH_SIZE len) FKS_NOEXCEPT;

/** �t�@�C�����̑召��r. ���l���������ꍇ�A�����Ⴂ�̐��l���m�̑召�𔽉f
 *	win/dos�n�͑召���ꎋ. �f�B���N�g���Z�p���[�^ \ / �����ꎋ.
 *	�ȊO�͒P���ɕ������r.
 */
FKS_LIB_DECL(int)
fks_pathDigitCmp(const FKS_PATH_CHAR* l, const FKS_PATH_CHAR* r) FKS_NOEXCEPT
{
	return fks_pathNDigitCmp(l, r, (FKS_PATH_SIZE)-1);
}

/** �� len ��蒷��������ŁAlen�����ڂ� ���l��̓r���������ꍇ�Alen�𒴂���strtol���Ă��܂�����
 *	   �Ӑ}�������ʂɂȂ�Ȃ��ꍇ������B�̂ŁAfnameNDigitCmp�͌��J�����T�u���[�`���Ƃ���.
 */
FKS_STATIC_DECL (int)
fks_pathNDigitCmp(const FKS_PATH_CHAR* l, const FKS_PATH_CHAR* r, FKS_PATH_SIZE len) FKS_NOEXCEPT
{
	const FKS_PATH_CHAR* e = l + len;
	FKS_ASSERT( l != 0 && r != 0 );
	if (e < l)
		e = (const FKS_PATH_CHAR*)-1;
	while (l < e) {
		ptrdiff_t	n;
		unsigned	lc;
		unsigned	rc;

		FKS_PATH_GET_C(lc, l);
		FKS_PATH_GET_C(rc, r);

		if (lc <= 0x80 && FKS_PATH_IS_DIGIT(lc) && rc <= 0x80 && FKS_PATH_IS_DIGIT(rc)) {
			FKS_LLONG	lv = FKS_PATH_STRTOLL(l - 1, (FKS_PATH_CHAR**)&l, 10);
			FKS_LLONG	rv = FKS_PATH_STRTOLL(r - 1, (FKS_PATH_CHAR**)&r, 10);
			FKS_LLONG	d  = lv - rv;
			if (d == 0)
				continue;
			return (d < 0) ? -1 : 1;
		}

	 #ifdef FKS_PATH_IGNORECASE
		lc = FKS_PATH_TO_LOWER(lc);
		rc = FKS_PATH_TO_LOWER(rc);
	 #endif

		n  = (ptrdiff_t)(lc - rc);
		if (n == 0) {
			if (lc == 0)
				return 0;
			continue;
		}

	  #ifdef FKS_WIN
		if ((lc == FKS_PATH_C('/') && rc == FKS_PATH_C('\\')) || (lc == FKS_PATH_C('\\') && rc == FKS_PATH_C('/')))
			continue;
	  #endif
		return (n < 0) ? -1 : 1;
	}
	return 0;
}



/** fname��prefix�Ŏn�܂��Ă���΁Afname�̗]���̐擪�̃A�h���X��Ԃ�.
 *	�}�b�`���Ă��Ȃ����NULL��Ԃ�.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathStartsWith(FKS_PATH_const_CHAR* fname, const FKS_PATH_CHAR* prefix) FKS_NOEXCEPT
{
	FKS_PATH_SIZE l;
	FKS_ASSERT(fname && prefix);
	l = fks_pathLen(prefix);
	if (l == 0)
		return (FKS_PATH_CHAR*)fname;
	return (fks_pathNCmp(fname, prefix, l) == 0) ? (FKS_PATH_CHAR*)fname+l : 0;
}


/** �g���q�̈ʒu��Ԃ�. '.'�͊܂�. �Ȃ���Ε�����̍Ō��Ԃ�.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathExt(FKS_PATH_const_CHAR* name) FKS_NOEXCEPT
{
	const FKS_PATH_CHAR *p;
	FKS_ARG_PTR_ASSERT(1, name);
	name = fks_pathBaseName(name);
	p	 = FKS_PATH_R_STR(name, FKS_PATH_C('.'));
	if (p)
		return (FKS_PATH_CHAR*)(p);

	return (FKS_PATH_CHAR*)name + fks_pathLen(name);
}


/** �t�@�C���p�X�����̊g���q���폜����.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathDelExt(FKS_PATH_CHAR buf[]) FKS_NOEXCEPT
{
	FKS_PATH_CHAR *t;
	FKS_PATH_CHAR *p;
	FKS_ARG_PTR_ASSERT(1, buf);
	t = fks_pathBaseName(buf);
	p = FKS_PATH_R_STR(t, FKS_PATH_C('.'));
	if (p == 0)
		p = t + fks_pathLen(t);
	*p = 0;
	return buf;
}


/** �t�@�C���p�X�����̊g���q�������������̎擾.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathGetNoExt(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, const FKS_PATH_CHAR *src) FKS_NOEXCEPT
{
	const FKS_PATH_CHAR *s;
	const FKS_PATH_CHAR *e;
	FKS_PATH_SIZE		  l = 0;
	FKS_ARG_PTR_ASSERT(1, dst);
	FKS_RANGE_UINTPTR_ASSERT(2, 2, (FKS_PATH_SIZE)-1);
	FKS_ARG_PTR_ASSERT(3, src);
	//if (dst == 0 || size == 0 || src == 0) return 0;
	s = fks_pathBaseName(src);
	e = FKS_PATH_R_STR(s, FKS_PATH_C('.'));
	if (e == 0)
		e = s + fks_pathLen(s);
	//l = e - src + 1;
	l = e - src;
	if (l > size)
		l = size;
	fks_pathCpy(dst, l, src);
	return dst;
}


/** �g���q���Aext �ɕύX����. dst == src �ł��悢.
 *	ext = NULL or "" �̂Ƃ��� �g���q�폜.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathSetExt(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, const FKS_PATH_CHAR* src, const FKS_PATH_CHAR *ext) FKS_NOEXCEPT
{
	FKS_ASSERT(dst != 0 && size > 0 && src != 0);
	fks_pathGetNoExt(dst,	size, src);
	if (ext && ext[0]) {
		if (ext[0] != FKS_PATH_C('.'))
			fks_pathCat(dst, size, FKS_PATH_C("."));
		fks_pathCat(dst, size, ext);
	}
	return dst;
}


/** �g���q���Ȃ��ꍇ�A�g���q��ǉ�����.(����Ή������Ȃ�). dst == src �ł��悢.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathSetDefaultExt(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, const FKS_PATH_CHAR* src, const FKS_PATH_CHAR *ext) FKS_NOEXCEPT
{
	FKS_PATH_CHAR* p;
	FKS_ASSERT(dst != 0 && size > 0 && src != 0);

	fks_pathCpy(dst, size, src);
	if (ext == 0)
		return dst;
	p = fks_pathBaseName(dst);
	p = FKS_PATH_R_STR(p, FKS_PATH_C('.'));
	if (p) {
		if (p[1])
			return dst;
		*p = 0;
	}
	if (ext[0]) {
		if (ext[0] != FKS_PATH_C('.'))
			fks_pathCat(dst, size, FKS_PATH_C("."));
		fks_pathCat(dst, size, ext);
	}
	return dst;
}


/** ������̍Ō�� \ �� / ������΂��̈ʒu��Ԃ��A�Ȃ����NULL��Ԃ�.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathCheckLastSep(FKS_PATH_const_CHAR* dir) FKS_NOEXCEPT
{
	FKS_PATH_SIZE l = fks_pathLen(dir);
	if (l == 0) return 0;
	return fks_pathCheckPosSep(dir, (ptrdiff_t)l - 1);
}


/** ������̎w��ʒu�� \ �� / ������΂��̈ʒu��Ԃ��A�Ȃ����NULL��Ԃ�.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathCheckPosSep(FKS_PATH_const_CHAR* dir, ptrdiff_t ofs) FKS_NOEXCEPT
{
	FKS_ASSERT(dir != 0);
	if (dir) {
		const FKS_PATH_CHAR*		s	= dir;
		if (ofs >= 0) {
			const FKS_PATH_CHAR*	p	= s + ofs;
			if (*p == FKS_PATH_C('/'))
				return (FKS_PATH_CHAR *)p;
		  #if (defined FKS_PATH_WINDOS)
			else if (*p == FKS_PATH_C('\\')) {
			  #ifdef FKS_PATH_WCS_COMPILE
				return (FKS_PATH_CHAR *)p;
			  #else 	// adjust_size�̌��ʂ�ofs�����ɂȂ��Ă���*p�̓}���`�o�C�g�����̈ꕔ.
				if (FKS_PATH_ADJUSTSIZE(s, ofs) == (FKS_PATH_SIZE)ofs)
					return (FKS_PATH_CHAR *)p;
			  #endif
			}
		  #endif
		}
	}
	return NULL;
}


/** ������̍Ō�� \ �� / ������΍폜
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathDelLastSep(FKS_PATH_CHAR dir[]) FKS_NOEXCEPT
{
	FKS_PATH_CHAR* p = fks_pathCheckLastSep(dir);
	if (p)
		*p = 0;
	return dir;
}


/** ������̍Ō�� �f�B���N�g���Z�p���[�^�������Ȃ���Βǉ�.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathAddSep(FKS_PATH_CHAR dir[], FKS_PATH_SIZE size) FKS_NOEXCEPT
{
	FKS_PATH_CHAR* e = dir + size;
	FKS_PATH_CHAR* p = fks_pathCheckLastSep(dir);
	if (p == 0) {
		p = dir + fks_pathLen(dir);
		if (p+1 < e) {
			*p++ = FKS_PATH_SEP_CHR;
			*p = 0;
		}
	}
	return dir;
}


/** �h���C�u�������Ă��邩.
 */
FKS_LIB_DECL (int)
fks_pathHasDrive(const FKS_PATH_CHAR* path) FKS_NOEXCEPT
{
	// �擪��"������:"���h���C�u���Ƃ݂Ȃ�.
	const FKS_PATH_CHAR* s = path;
	if (s == 0)
		return 0;
	if (*s && *s != FKS_PATH_C(':')) {
		while (*s && !fks_pathIsSep(*s)) {
			if (*s == FKS_PATH_C(':'))
				return 1;
			++s;
		}
	}
	return 0;
}


/** �h���C�u��������΂�����X�L�b�v�����|�C���^��Ԃ�.
 *	 �� c:�������łȂ�http:���X�L�b�v���邽�� "������:" ���X�L�b�v.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathSkipDrive(FKS_PATH_const_CHAR* path) FKS_NOEXCEPT
{
	const FKS_PATH_CHAR* s = path;
	if (*s && *s != FKS_PATH_C(':')) {
		while (*s && !fks_pathIsSep(*s)) {
			if (*s == FKS_PATH_C(':'))
				return (FKS_PATH_CHAR*)s+1;
			++s;
		}
	}
	return (FKS_PATH_CHAR*)path;
}


/** ��΃p�X���ۂ�(�h���C�u���̗L���͊֌W�Ȃ�)
 */
FKS_LIB_DECL (int)
fks_pathIsAbs(const FKS_PATH_CHAR* path) FKS_NOEXCEPT 
{
	if (path == 0)
		return 0;
	path = fks_pathSkipDrive(path);
	return fks_pathIsSep(path[0]);
}


/** �����R�[�h���l������  strlwr.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathToLowerN(FKS_PATH_CHAR name[], size_t n) FKS_NOEXCEPT
{
 #ifdef FKS_STRLWR_N
	return FKS_STRLWR_N(name, n);
 #else
	FKS_PATH_CHAR *p = name;
	FKS_PATH_CHAR *e = p + n;
	FKS_ASSERT(name != NULL);

	while (p < e) {
		unsigned c = *p;
		*p = FKS_PATH_TO_LOWER(c);
		p  = FKS_PATH_CHARNEXT(p);
	}
	return name;
 #endif
}

FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathToLower(FKS_PATH_CHAR name[]) FKS_NOEXCEPT
{
 #ifdef FKS_STRLWR
	return FKS_STRLWR(name);
 #else
	return fks_pathToLowerN(name, fks_pathLen(name));
 #endif
}


/** �����R�[�h���l������  strupr.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathToUpperN(FKS_PATH_CHAR name[], size_t n) FKS_NOEXCEPT
{
 #ifdef FKS_STRUPR_N
	return FKS_STRUPR_N(name, n);
 #else
	FKS_PATH_CHAR *p = name;
	FKS_PATH_CHAR *e = p + n;
	FKS_ASSERT(name != NULL);

	while (p < e) {
		unsigned c = *p;
		*p = FKS_PATH_TO_UPPER(c);
		p  = FKS_PATH_CHARNEXT(p);
	}
	return name;
 #endif
}

FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathToUpper(FKS_PATH_CHAR name[]) FKS_NOEXCEPT
{
 #ifdef FKS_STRUPR
	return FKS_STRUPR(name);
 #else
	return fks_pathToUpperN(name, fks_pathLen(name));
 #endif
}


/** �t�@�C���p�X�����̃f�B���N�g���Ɗg���q���������t�@�C�����̎擾.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathGetBaseNameNoExt(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, const FKS_PATH_CHAR *src) FKS_NOEXCEPT
{
	const FKS_PATH_CHAR *s;
	const FKS_PATH_CHAR *e;
	FKS_PATH_SIZE		  l = 0;
	FKS_ASSERT(dst != 0 && size > 1 && src != 0);
	//if (dst == 0 || size == 0 || src == 0) return 0;
	s = fks_pathBaseName(src);
	e = FKS_PATH_R_STR(s, FKS_PATH_C('.'));
	if (e == 0)
		e = s + fks_pathLen(s);
	l = e - s; // +1;
	if (l > size)
		l = size;
	fks_pathCpy(dst, l, s);
	return dst;
}


/** �f�B���N�g��������Ԃ�. �Ō�̃f�B���N�g���Z�p���[�^�͊O��.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathGetDir(FKS_PATH_CHAR dir[], FKS_PATH_SIZE size, const FKS_PATH_CHAR *name) FKS_NOEXCEPT
{
	const FKS_PATH_CHAR*	p;
	size_t					l;

	FKS_ASSERT(dir	!= 0 && size > 0 && name != 0);

	p = fks_pathBaseName(name);
	l = p - name; // +1;
	if (l > size)
		l = size;
	if (l && dir != name)
		fks_pathCpy(dir, l, name);
	dir[l] = 0;
	if (l > 0)
		fks_pathDelLastSep(dir);
	return dir;
}


/** �h���C�u���������擾. :��. �� file:���̑Ώ��̂���"������:"���h���C�u����
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathGetDrive(FKS_PATH_CHAR drive[], FKS_PATH_SIZE size, const FKS_PATH_CHAR *name) FKS_NOEXCEPT
{
	const FKS_PATH_CHAR*	s;
	FKS_PATH_SIZE			l;
	FKS_ASSERT(drive && size > 0 && name);
	drive[0] = 0;
	s = fks_pathSkipDrive(name);
	l = s - name;
	if (l > 0) {
		//++l;
		if (l > size)
			l = size;
		fks_pathCpy(drive, l,	name);
	}
	return drive;
}


/** �h���C�u���ƃ��[�g�w�蕔�����擾.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathGetDriveRoot(FKS_PATH_CHAR dr[], FKS_PATH_SIZE size, const FKS_PATH_CHAR *name) FKS_NOEXCEPT
{
	const FKS_PATH_CHAR*	s;
	FKS_PATH_SIZE			l;
	FKS_ASSERT(dr && size > 0 && name);
	dr[0] = 0;
	s = fks_pathSkipDriveRoot(name);
	l = s - name;
	if (l > 0) {
		//++l;
		if (l > size)
			l = size;
		fks_pathCpy(dr, l, name);
	}
	return dr;
}


/** ������擪�̃h���C�u��,���[�g�w����X�L�b�v�����|�C���^��Ԃ�.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathSkipDriveRoot(FKS_PATH_const_CHAR* path) FKS_NOEXCEPT
{
	FKS_PATH_CHAR* p = fks_pathSkipDrive(path);
	while (fks_pathIsSep(*p))
		++p;
	return p;
}


/** filePath���� \ �� / �ɒu��.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathBackslashToSlash(FKS_PATH_CHAR filePath[]) FKS_NOEXCEPT
{
	FKS_PATH_CHAR *p = filePath;
	FKS_ASSERT(filePath != NULL);
	while (*p != FKS_PATH_C('\0')) {
		if (*p == FKS_PATH_C('\\')) {
			*p = FKS_PATH_C('/');
		}
		p = FKS_PATH_CHARNEXT(p);
	}
	return filePath;
}


/** filePath���� / �� \ �ɒu��.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathSlashToBackslash(FKS_PATH_CHAR filePath[]) FKS_NOEXCEPT
{
	FKS_PATH_CHAR *p;
	FKS_ASSERT(filePath != NULL);
	for (p = filePath; *p; ++p) {
		if (*p == FKS_PATH_C('/'))
			*p = FKS_PATH_C('\\');
	}
	return filePath;
}


FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathFullpath(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, const FKS_PATH_CHAR* path, const FKS_PATH_CHAR* currentDir)  FKS_NOEXCEPT
{
  #if defined FKS_WINDOS
	return fks_pathFullpathBS(dst, size, path, currentDir);
  #else
	return fks_pathFullpathSL(dst, size, path, currentDir);
  #endif
}


/** �t���p�X����. �f�B���N�g���Z�p���[�^��\\�ɂ��ĕԂ��o�[�W����.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathFullpathBS(FKS_PATH_CHAR	dst[], FKS_PATH_SIZE size, const FKS_PATH_CHAR* path, const FKS_PATH_CHAR* currentDir) FKS_NOEXCEPT
{
	fks_pathFullpathSL(dst, size,	path, currentDir);
	fks_pathSlashToBackslash(dst);
	return dst;
}


/** �t���p�X����. �����񑀍�̂�. �J�����g�p�X�͈����œn��.
 *	currentDir �͐�΃p�X�ł��邱��. �����łȂ��ꍇ�̋����͕s��.
 *	'\'�����΍�ŁA�Z�p���[�^��'/'�ɒu�����Ă���.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathFullpathSL(FKS_PATH_CHAR	dst[], FKS_PATH_SIZE size, const FKS_PATH_CHAR* path, const FKS_PATH_CHAR* currentDir) FKS_NOEXCEPT
{
	FKS_PATH_CHAR* 		wk;
	FKS_PATH_SIZE		wkSz;

	FKS_ASSERT(dst != 0 && size > 2 && path != 0);
	if (dst == 0 || size <= 2 || path == 0)
		return 0;
	if (currentDir == 0)
		currentDir = FKS_PATH_C("/");	// DIRSEP_STR;
	FKS_ASSERT(fks_pathIsAbs(currentDir));

  #ifdef FKS_PATH_USE_ALLOCA
	// �������ݐ�T�C�Y���\���łȂ���΍�Ɨp���������m��.
	{
		FKS_PATH_SIZE pl = fks_pathLen(path);
		FKS_PATH_SIZE cl = fks_pathLen(currentDir);
		wkSz = pl + cl + 4;
		if (wkSz >= size) { 	// dst�T�C�Y�����A���������Ȃ烏�[�N��p��.
			FKS_ASSERT( wkSz <= FKS_PATH_MAX_URL * sizeof(FKS_PATH_CHAR) );
			wk = (FKS_PATH_CHAR*)fks_alloca(wkSz*sizeof(FKS_PATH_CHAR));
			if (wk == 0) {
				wk	 = dst;
				wkSz = size;
			}
		} else {
			wkSz = size;
			wk	 = dst;
		}
	}
  #else // alloca��S���g��Ȃ��ꍇ�͏o�͐�𒼐ڎg������.
	wkSz = size;
	wk	 = dst;
  #endif

	// ��Ɨp�̐�΃p�X�����.
	{
		unsigned hasDrive = fks_pathHasDrive(path);
		unsigned isAbs	  = fks_pathIsAbs(path);
		wk[0] = 0;
		if (isAbs && hasDrive) {	// �h���C�u�t����΃p�X�Ȃ�A���̂܂�.
			fks_pathCpy(wk, wkSz,	path);
		} else if (isAbs) {
			if (fks_pathHasDrive(currentDir))	// ��΃p�X�����ǃh���C�u���Ȃ��ꍇ��currentDir����h���C�u������������.
				fks_pathGetDrive(wk, wkSz, currentDir);
			fks_pathCat(wk, wkSz,	path);
		} else {
			if (hasDrive) { 		// �h���C�u�t�����΃p�X�ŁA
				if (!fks_pathHasDrive(currentDir)) // �J�����g���Ƀh���C�u���Ȃ����
					fks_pathGetDrive(wk, wkSz, path);	// path�̃h���C�u����ݒ�. �������΃J�����g���̃h���C�u���ɂȂ�.
			}
			fks_pathCat(wk, wkSz,	currentDir);
			fks_pathAddSep(wk, wkSz);
			fks_pathCat(wk, wkSz,	fks_pathSkipDrive(path));
		}
	}

  #if defined FKS_WIN
	// �������ȒP�ɂ��邽�߁A�p�X�̋�؂����U / �ɕϊ�.
	fks_pathBackslashToSlash(wk);
  #endif

	// "." �� ".." ����菜��.
	{
		// ���̎��_��wk�͕K����΃p�X�ɂȂ��Ă���.(currentDir���ᔽ���Ă��ꍇ�̋����͕s�舵��).
		FKS_PATH_CHAR* 	s	  = fks_pathSkipDrive(wk); // �h���C�u���͘M��Ȃ��̂ŃX�L�b�v.
		FKS_PATH_CHAR* 	d	  = s;
		FKS_PATH_CHAR* 	top   = d;
		unsigned		first = 1;
		while (*s) {
			unsigned c = *s++;
			if (c == FKS_PATH_C('/')) {
				if (first) {	// ����� / �� "//" "///" ������... ���Ƃ�*d++=c����̂ł����ł�2��܂�.
					unsigned i;
					for (i = 0; i < 2 && *s == FKS_PATH_C('/'); ++i)
						*d++ = *s++;
				}
				first = 0;
				// '/'�̘A���͈��'/'����.
			  RETRY:
				while (*s == FKS_PATH_C('/'))
					++s;
				if (*s == FKS_PATH_C('.')) {
					if (s[1] == 0) {					// "." �݂͖̂���.
						s += 1;
						goto RETRY;
					} else if (s[1] == FKS_PATH_C('/')) {		// "./" �͖���.
						s += 2;
						goto RETRY;
					} else if (s[1] == FKS_PATH_C('.') && (s[2] == FKS_PATH_C('/') || s[2] == 0)) {	// "../" ".." �̂Ƃ�.
						s += 2 + (s[2] != 0);
						while (d > top && *--d != FKS_PATH_C('/'))	// �o�͐�̃f�B���N�g���K�w���P���炷.
							;
						goto RETRY;
					}
				}
			}
			*d++ = c;
		}
		*d = 0;
	}

  #ifdef FKS_PATH_USE_ALLOCA
	if (wk != dst)	// ���[�N��alloca���Ă��̂Ȃ�A�R�s�[.
		fks_pathCpy(dst, size, wk);
  #endif

	return dst;
}


FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathRelativePath(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, const FKS_PATH_CHAR* path, const FKS_PATH_CHAR* currentDir)  FKS_NOEXCEPT
{
  #if defined FKS_WIN
	return fks_pathRelativePathBS(dst, size, path, currentDir);
  #else
	return fks_pathRelativePathSL(dst, size, path, currentDir);
  #endif
}

/** ���΃p�X����. �f�B���N�g���Z�p���[�^��\\�ɂ��ĕԂ��o�[�W����.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathRelativePathBS(FKS_PATH_CHAR	dst[], FKS_PATH_SIZE size, const FKS_PATH_CHAR* path, const FKS_PATH_CHAR* currentDir) FKS_NOEXCEPT
{
	fks_pathRelativePathSL(dst, size,	path, currentDir);
	fks_pathSlashToBackslash(dst);
	return dst;
}


/** currentDir����̑��΃p�X����.
 *	currentDir �͐�΃p�X�ł��邱��. �����łȂ��ꍇ�̋����͕s��.
 *	'\'�����΍�ŁA�Z�p���[�^��'/'�ɒu�����Ă���.
 *	�J�����g�f�B���N�g������n�܂�ꍇ�A"./"�͂��Ȃ�.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathRelativePathSL(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, const FKS_PATH_CHAR* path, const FKS_PATH_CHAR* currentDir) FKS_NOEXCEPT
{
	enum { CHECK_MAX_PATH = sizeof(char[FKS_PATH_MAX >= 16 ? 1 : -1]) };	// �R���p�C�����̃T�C�Y�`�F�b�N.
	enum { CHECK_MAX_URL  = sizeof(char[FKS_PATH_MAX_URL  >= 16 ? 1 : -1]) };	// �R���p�C�����̃T�C�Y�`�F�b�N.
	FKS_PATH_CHAR		curDir	[ FKS_PATH_MAX_URL + 1 ];
	FKS_PATH_CHAR		fullName[ FKS_PATH_MAX_URL + 1 ];
	FKS_PATH_CHAR* 		cp;
	FKS_PATH_CHAR* 		cpSav;
	FKS_PATH_CHAR* 		fp;
	FKS_PATH_SIZE		cl;
	FKS_PATH_SIZE		fl;

	FKS_ASSERT(dst != 0 && size > 2 && path != 0);
	if (dst == 0 || size <= 2 || path == 0)
		return 0;

	// �܂��A�J�����g�f�B���N�g�����t���p�X��(/) & �Ō��/��t��.
	FKS_ASSERT(fks_pathIsAbs(currentDir));
	fks_pathFullpathSL(curDir, FKS_PATH_MAX_URL,	currentDir, FKS_PATH_C("/"));
	cp = fks_pathCheckLastSep(curDir);
	if (cp == 0) {
		cp = curDir + fks_pathLen(curDir);
		if (cp+1 < curDir+FKS_PATH_MAX_URL) {
			*cp++ = FKS_PATH_C('/');
			*cp = 0;
		}
	}

	// �Ώۂ� path ���t���p�X��. \\�͖ʓ|�Ȃ̂�/���������.
	fks_pathFullpathSL(fullName, FKS_PATH_MAX_URL, path, curDir);

	// �}�b�`���Ă���f�B���N�g���������X�L�b�v.
	cp	  = fks_pathSkipDriveRoot(curDir);
	fp	  = fks_pathSkipDriveRoot(fullName);

	// �h���C�u��������Ă����瑊�΃p�X�����Ȃ�.
	cl	  = cp - curDir;
	fl	  = fp - fullName;
	if (cl != fl || fks_pathNCmp(curDir,	fullName, fl) != 0) {
		fks_pathCpy(dst, size, fullName);
		return dst;
	}

	// �����������`�F�b�N.
	cpSav = cp;
	while (*cp && *fp) {
		unsigned cc;
		unsigned fc;
		FKS_PATH_GET_C(cc, cp);
		FKS_PATH_GET_C(fc, fp);
		if (cc != fc)
			break;
		if (*cp == FKS_PATH_C('/'))
			cpSav = (FKS_PATH_CHAR*)cp + 1;
	}
	fp		= fp - (cp - cpSav);
	cp		= cpSav;

	// �J�����g�ʒu�����ւ̈ړ�����../�𐶐�.
	{
		FKS_PATH_CHAR* d = dst;
		FKS_PATH_CHAR* e = dst + size - 1;
		while (*cp) {
			if (*cp == FKS_PATH_C('/')) {
				if (d < e) *d++ = FKS_PATH_C('.');
				if (d < e) *d++ = FKS_PATH_C('.');
				if (d < e) *d++ = FKS_PATH_C('/');
			}
			++cp;
		}
		*d = FKS_PATH_C('\0');
	}

	// �J�����g�ʒu�ȉ��̕������R�s�[
	fks_pathCat(dst, size, fp);

	return dst;
}


/** ?,* �݂̂�(dos/win��)���C���h�J�[�h�������r.
 * *,? �̓Z�p���[�^�ɂ̓}�b�`���Ȃ�.
 * �������g������ ** �̓Z�p���[�^�ɂ��}�b�`����.
 * ��unDonut�̃\�[�X�Q�l.
 *	 ���̌���http://www.hidecnet.ne.jp/~sinzan/tips/main.htm�炵���������N��.
 *	@param ptn	�p�^�[��(*?�w��\)
 *	@param tgt	�^�[�Q�b�g������.
 */
FKS_LIB_DECL (int)
fks_pathMatchWildCard(const FKS_PATH_CHAR* ptn, const FKS_PATH_CHAR* tgt) FKS_NOEXCEPT
{
	unsigned				tc;
	const FKS_PATH_CHAR*	tgt2 = tgt;
	FKS_PATH_GET_C(tc, tgt2);	// 1���擾& tgt�|�C���^�X�V.
	switch (*ptn) {
	case FKS_PATH_C('\0'):
		return tc == FKS_PATH_C('\0');

  #if defined FKS_PATH_WINDOWS
	case FKS_PATH_C('\\'):
  #endif
	case FKS_PATH_C('/'):
		return fks_pathIsSep(tc) && fks_pathMatchWildCard(ptn+1, tgt2);

	case FKS_PATH_C('?'):
		return tc && !fks_pathIsSep(tc) && fks_pathMatchWildCard( ptn+1, tgt2 );

	case FKS_PATH_C('*'):
		if (ptn[1] == FKS_PATH_C('*')) // ** �Ȃ�Z�p���[�^�ɂ��}�b�`.
			return fks_pathMatchWildCard(ptn+2, tgt) || (tc && fks_pathMatchWildCard( ptn, tgt2	));
		return fks_pathMatchWildCard(ptn+1, tgt) || (tc && !fks_pathIsSep(tc) && fks_pathMatchWildCard( ptn, tgt2	));

	default:
		{
			unsigned pc;
			FKS_PATH_GET_C(pc, ptn);	// 1���擾& ptn�|�C���^�X�V.
			return (pc == tc) && fks_pathMatchWildCard(ptn, tgt2);
		}
	}
}


/** �R�}���h���C��������A;��؂�̕����p�X�w����A��������̂Ɏg��.
 *	""��win�R�}���h���C���ɂ��킹������.
 *	sepChr�ŋ�؂�ꂽ������(�t�@�C����)���擾. 0x20�ȊO�̋󔒂͖�����0x20����.
 *	@return �X�L�����X�V��̃A�h���X��Ԃ��Bstr��EOS��������NULL��Ԃ�.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathScanArgStr(FKS_PATH_CHAR arg[], FKS_PATH_SIZE argSz, const FKS_PATH_CHAR *str, unsigned sepChr) FKS_NOEXCEPT
{
  #ifdef FKS_PATH_WCS_COMPILE
	const FKS_PATH_CHAR*	s = str;
  #else
	const unsigned char*	s = (const unsigned char*)str;
  #endif
	FKS_PATH_CHAR* 		d = arg;
	FKS_PATH_CHAR* 		e = d + argSz;
	unsigned				f = 0;
	unsigned				c;

	FKS_ASSERT( str != 0 && arg != 0 && argSz > 1 );

	// 0x20�ȊO�̋󔒂ƃZ�p���[�^���X�L�b�v.
	while ( *s == sepChr || (*s > 0U && *s < 0x20U) || *s == 0x7fU)
		++s;
	if (*s == 0) {	// EOS��������A������Ȃ������Ƃ���NULL��Ԃ�.
		arg[0] = 0;
		return NULL;
	}

	do {
		c = *s++;
		if (c == FKS_PATH_C('"')) {
			f ^= 1; 						// "�̑΂̊Ԃ͋󔒂��t�@�C�����ɋ���.���߂̃t���O.

			// ������ƋC�����������AWin(XP)��cmd.exe�̋����ɍ��킹�Ă݂�.
			// (�ق�Ƃɂ����Ă邩�A�\���ɂ͒��ׂĂȂ�)
			if (*s == FKS_PATH_C('"') && f == 0)	// ��"�̒���ɂ����"������΁A����͂��̂܂ܕ\������.
				++s;
			else
				continue;					// �ʏ�� " �͏Ȃ��Ă��܂�.
		}
		if ((c > 0 && c < 0x20) || c == 0x7f)
			c = FKS_PATH_C(' ');
		if (d < e)
			*d++ = (FKS_PATH_CHAR)c;
	} while (c >= 0x20 && c != 0x7f && (f || (c != sepChr)));
	*--d  = FKS_PATH_C('\0');
	--s;
	return (FKS_PATH_CHAR *)s;
}


#if (defined __cplusplus) && !(defined FKS_PATH_WCS_COMPILE)
}
#endif
