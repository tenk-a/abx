/**
 *	@file	fks_priv_fname_hdr.h
 *	@brief	�t�@�C���������֌W
 *	@author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 *	@note
 *	-	c/c++ �p.
 *	-	win / linux(unix) �p.
 *		win�ł�\ �� /���A�ȊO��/���Z�p���[�^.
 *		�ꉞdos���肵�Ă��邪�Aint=2byte���l�����Ă��Ȃ��̂Œ���.
 *	-	������0x80������ascii�n�ł��邱�ƑO��.
 *	-	SJIS���̃}���`�o�C�g�����Ή�.
 *		- Win���ł̓}���`�o�C�g������CharNext�Ń|�C���^��i�߂Ă���.
 *		  (CharNext��UTF8�Ή����Ă������悤��.�ł�win���̂�����MBC��DBC�̂�
 *		  ��utf8�Ή����Ă��Ȃ�)
 *		- Win�ȊO�̊��ł́A���ϐ�LANG���݂�, SJIS,BIG5,GBK���l��.
 *		  (�����炭�S�p2�o�C�g�ڂ�0x5c������̂́A���ꂭ�炢?�Ƒz��)
 *		- ��r�֌W�́A���P�[���Ή��s�\��
 */

#include <fks/fks_config.h>
#include <fks/fks_fname.h>
#include <fks/fks_assert_ex.h>
#include <fks/fks_alloca.h>

//#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// os �̈Ⴂ�֌W.
#if defined _WIN32
 #include <windows.h>
 #if defined _MSC_VER
  #pragma comment(lib, "User32.lib")			// CharNext()�ŕK�v...
 #endif
#endif

// fullpath ����alloca���g���ꍇ�ꍇ��`.
#define FKS_FNAME_USE_ALLOCA

// unicode�Ή�. �� mb�n���g��ꂽ���Ȃ����� tchar.h ���g�킸���O�őΏ�.
#ifdef FKS_FNAME_WCS_COMPILE	// wchar_t �Ή�.
 #define FKS_FNAME_C(x) 			L##x
 #define FKS_FNAME_CHAR 			wchar_t
 #define FKS_FNAME_R_STR(s,c)		wcsrchr((s),(c))
 #if defined FKS_FNAME_WINDOS		// �召�������ꎋ.
  #ifdef _WIN32	// _MSC_VER
   #define FKS_FNAME_CMP(l,r)		_wcsnicmp((l),(r))
   #define FKS_FNAME_N_CMP(l,r,n)	_wcsnicmp((l),(r),(n))
  #else
   #define FKS_FNAME_CMP(l,r)		wcscasecmp((l),(r))
   #define FKS_FNAME_N_CMP(l,r,n)	wcsncasecmp((l),(r),(n))
  #endif
 #else								// �召���.
  #define FKS_FNAME_CMP(l,r)		wcscmp((l),(r))
  #define FKS_FNAME_N_CMP(l,r,n)	wcsncmp((l),(r),(n))
 #endif
 #define FKS_FNAME_STRTOL(s,t,r)	wcstoll((s),(t),(r))
 #if _WIN32
  #define FKS_STRUPR(s)				CharUpperW(s)
  #define FKS_STRLWR(s)				CharLowerW(s)
 #endif
#else			// char �x�[�X.
 #define FKS_FNAME_C(x) 			x
 #define FKS_FNAME_CHAR 			char
 #define FKS_FNAME_R_STR(s,c)		strrchr((s),(c))
 #if defined FKS_FNAME_WINDOS		// �召�������ꎋ.
  #ifdef _WIN32	// _MSC_VER
   #define FKS_FNAME_CMP(l,r)		_strnicmp((l),(r))
   #define FKS_FNAME_N_CMP(l,r,n)	_strnicmp((l),(r),(n))
  #else
   #define FKS_FNAME_CMP(l,r)		strcasecmp((l),(r))
   #define FKS_FNAME_N_CMP(l,r,n)	strncasecmp((l),(r),(n))
  #endif
 #else								// �召���.
  #define FKS_FNAME_CMP(l,r)		strcmp((l),(r))
  #define FKS_FNAME_N_CMP(l,r,n)	strncmp((l),(r),(n))
 #endif
 #define FKS_FNAME_STRTOL(s,t,r)	strtoll((s),(t),(r))
 #if _WIN32
  #define FKS_STRUPR(s)				CharUpperA(s)
  #define FKS_STRLWR(s)				CharLowerA(s)
 #endif
#endif
#define FKS_FNAME_IS_DIGIT(c)		(('0') <= (c) && (c) <= ('9'))

// c/c++ �΍�.
#ifdef __cplusplus										// c++�̏ꍇ�A�|�C���^����݂̂̊֐���const��,��const�ł�����.
  #define FKS_FNAME_const_CHAR		FKS_FNAME_CHAR		// ���̂��߁A��{�́A��const�֐��ɂ���.
#else													// c�̏ꍇ�͕W�����C�u�����ɂ��킹 ����const�Ŗ߂�l ��const �ɂ���.
 #define FKS_FNAME_const_CHAR		const FKS_FNAME_CHAR
#endif

#if defined _WIN32 && defined(FKS_FNAME_WCS_COMPILE)
 #define FKS_FNAME_TO_UPPER(c)		(wchar_t)CharUpperW((wchar_t*)(c))
 #define FKS_FNAME_TO_LOWER(c)		(wchar_t)CharLowerW((wchar_t*)(c))
#else
 #define FKS_FNAME_TO_UPPER(c)		(((c) >= FKS_FNAME_C('a') && (c) <= FKS_FNAME_C('z')) ? (c) - FKS_FNAME_C('a') + FKS_FNAME_C('A') : (c))
 #define FKS_FNAME_TO_LOWER(c)		(((c) >= FKS_FNAME_C('A') && (c) <= FKS_FNAME_C('Z')) ? (c) - FKS_FNAME_C('A') + FKS_FNAME_C('a') : (c))
#endif

// �}���`�o�C�g������0x5c �΍�֌W.

/// ���� C �� MS�S�p�̂P�o�C�g�ڂ��ۂ�. (utf8��euc�� \ ���͖����̂� 0���A���ok)
#if defined FKS_FNAME_WCS_COMPILE
 #define FKS_FNAME_ISMBBLEAD(c) 	(0)
#elif defined _WIN32
 #define FKS_FNAME_ISMBBLEAD(c) 	IsDBCSLeadByte(c)
//#elif defined HAVE_MBCTYPE_H
// #define FKS_FNAME_ISMBBLEAD(c)	_ismbblead(c)
#elif defined FKS_USE_FNAME_MBC
 #define FKS_FNAME_ISMBBLEAD(c) 	((unsigned)(c) >= 0x80 && fks_fnameIsZenkaku1(c) > 0)
#else
 #define FKS_FNAME_ISMBBLEAD(c) 	(0)
#endif

/// ���̕����փ|�C���^��i�߂�. ��CharNext()���T���Q�[�g�y�A��utf8�Ή����Ă���Ă��炢���ȂƊ���(�ʖڂ���������)
#if  defined _WIN32
 #ifdef FKS_FNAME_WCS_COMPILE
  #define FKS_FNAME_CHARNEXT(p) 	(FKS_FNAME_CHAR*)CharNextW((FKS_FNAME_CHAR*)(p))
 #else
  #define FKS_FNAME_CHARNEXT(p) 	(FKS_FNAME_CHAR*)CharNextA((FKS_FNAME_CHAR*)(p))
 #endif
#else
 #if defined FKS_FNAME_WCS_COMPILE || !defined(FKS_USE_FNAME_MBC)
  #define FKS_FNAME_CHARNEXT(p) 	((p) + 1)
 #else
  #define FKS_FNAME_CHARNEXT(p) 	((p) + 1 + (FKS_FNAME_ISMBBLEAD(*(unsigned char*)(p)) && (p)[1]))
 #endif
#endif

// �t�@�C����������̃|�C���^p����1�����擾����c�ɂ����}�N��.
// os��win�Ȃ�2�o�C�g�����Ή��ŏ�������. utf8�͔j�].
#ifdef FKS_FNAME_WCS_COMPILE
 #if defined FKS_FNAME_WINDOS
  #define FKS_FNAME_GET_C(c, p) 	((c) = *((p)++), (c) = FKS_FNAME_TO_LOWER(c))
 #else
  #define FKS_FNAME_GET_C(c, p) 	((c) = *((p)++))
 #endif
#else
 #if defined FKS_FNAME_WINDOS
  #define FKS_FNAME_GET_C(c, p) do {						\
		(c) = *(unsigned char*)((p)++); 					\
		if (FKS_FNAME_ISMBBLEAD(c) && *(p)) 				\
			(c) = ((c) << 8) | *(unsigned char*)((p)++);	\
		else												\
			(c) = FKS_FNAME_TO_LOWER(c);					\
	} while (0)
 #else
  #define FKS_FNAME_GET_C(c, p) do {						\
		(c) = *(unsigned char*)((p)++); 					\
		if (FKS_FNAME_ISMBBLEAD(c) && *(p)) 				\
			(c) = ((c) << 8) | *(unsigned char*)((p)++);	\
	} while (0)
 #endif
#endif



#if (defined __cplusplus) && !(defined FKS_FNAME_WCS_COMPILE)
extern "C" {
#endif


/** �t�@�C���p�X�����̃f�B���N�g�����������t�@�C�����̈ʒu��Ԃ�.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameBaseName(FKS_FNAME_const_CHAR *adr) FKS_NOEXCEPT
{
	const FKS_FNAME_CHAR *p = adr;
	FKS_ARG_PTR_ASSERT(1, adr);
	while (*p) {
		if (*p == FKS_FNAME_C(':') || fks_fnameIsSep(*p))
			adr = (FKS_FNAME_CHAR*)p + 1;
		p = FKS_FNAME_CHARNEXT(p);
	}
	return (FKS_FNAME_CHAR*)adr;
}


/** size�Ɏ��܂镶����̕�������Ԃ�. \0���܂܂Ȃ�.
 *	(win���ł͂Ȃ�ׂ��}���`�o�C�g�����̓r���ŏI���Ȃ��悤�ɂ���.
 *	 ���ǁA�p�r�I�ɂ͐؂��ȏ゠�܂�Ӗ��Ȃ�...)
 */
FKS_LIB_DECL (FKS_FNAME_SIZE)
fks_fnameAdjustSize(const FKS_FNAME_CHAR* str, FKS_FNAME_SIZE size) FKS_NOEXCEPT
{
	const FKS_FNAME_CHAR* s = str;
	const FKS_FNAME_CHAR* b = s;
	const FKS_FNAME_CHAR* e = s + size;
	FKS_ARG_PTR_ASSERT(1, str);
	FKS_ARG_ASSERT(1, (size > 0));
	while (s < e) {
		if (*s == 0)
			return s - str;
		b = s;
		s = FKS_FNAME_CHARNEXT((FKS_FNAME_CHAR*)s);
	}
	if (s > e)
		s = b;
	return s - str;
}


/** �t�@�C�����̃R�s�[. mbc�̎��͕��������Ȃ������܂�. dst == src ��ok.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameCpy(FKS_FNAME_CHAR dst[], FKS_FNAME_SIZE dstSz, const FKS_FNAME_CHAR* src) FKS_NOEXCEPT
{
	FKS_FNAME_SIZE	l;
	FKS_ARG_PTR_ASSERT(1, dst);
	FKS_ARG_ASSERT(2, (dstSz > 0));
	FKS_ARG_PTR0_ASSERT(3, src);

	if (src == NULL)
		return NULL;
	l = fks_fnameAdjustSize(src, dstSz);

	// �A�h���X�������Ȃ�A���������킹��̂�.
	if (dst == src) {
		dst[l] = 0;
		return dst;
	}

	// �R�s�[.
	{
		const FKS_FNAME_CHAR*	s = src;
		const FKS_FNAME_CHAR*	e = s + l;
		FKS_FNAME_CHAR* 		d = dst;
		while (s < e)
			*d++ = *s++;
		*d = 0;
	}

	return dst;
}


/** �t�@�C����������̘A��.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameCat(FKS_FNAME_CHAR dst[], FKS_FNAME_SIZE dstSz, const FKS_FNAME_CHAR* src) FKS_NOEXCEPT
{
	FKS_FNAME_SIZE l;
	FKS_ARG_PTR_ASSERT(1, dst);
	FKS_ARG_ASSERT(2, (dstSz > 0));
	FKS_ARG_PTR0_ASSERT(3, src);
	if (src == 0)
		return NULL;
	FKS_ASSERT(src != 0 && dst != src);
	l = fks_fnameLen(dst);
	if (l >= dstSz)		// ���������]���悪���t�Ȃ�T�C�Y�����̂�.
		return fks_fnameCpy(dst, dstSz, dst);
	fks_fnameCpy(dst+l, dstSz - l,	src);
	return dst;
}


/** �f�B���N�g�����ƃt�@�C��������������. fks_fnameCat �ƈႢ�A\	/ ���Ԃɕt��.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameJoin(FKS_FNAME_CHAR buf[], FKS_FNAME_SIZE bufSz, const FKS_FNAME_CHAR *dir, const FKS_FNAME_CHAR *name) FKS_NOEXCEPT
{
	fks_fnameCpy(buf, bufSz, dir);
	if (buf[0])
		fks_fnameAddSep(buf, bufSz);
	fks_fnameCat(buf, bufSz, name);
	return buf;
}


/** �t�@�C�����̑召��r.
 *	win/dos�n�͑召���ꎋ. �f�B���N�g���Z�p���[�^ \ / �����ꎋ.
 *	�ȊO�͒P���ɕ������r.
 */
FKS_LIB_DECL (int)
fks_fnameCmp(const FKS_FNAME_CHAR* l,	const FKS_FNAME_CHAR* r) FKS_NOEXCEPT
{
	return fks_fnameNCmp(l, r, (FKS_FNAME_SIZE)-1);
}


/** �t�@�C�����̑召��r.
 *	win/dos�n�͑召���ꎋ. �f�B���N�g���Z�p���[�^ \ / �����ꎋ.
 *	�ȊO�͒P���ɕ������r.
 */
FKS_LIB_DECL (int)
fks_fnameNCmp(const FKS_FNAME_CHAR* l,	const FKS_FNAME_CHAR* r, FKS_FNAME_SIZE len) FKS_NOEXCEPT
{
 #if defined FKS_FNAME_WINDOS
	const FKS_FNAME_CHAR* e = l + len;
	FKS_ASSERT( l != 0 && r != 0 );
	if (e < l)
		e = (const FKS_FNAME_CHAR*)-1;
	while (l < e) {
		int 	 n;
		unsigned lc;
		unsigned rc;

		FKS_FNAME_GET_C(lc, l);
		FKS_FNAME_GET_C(rc, r);

		n  = (int)(lc - rc);
		if (n == 0) {
			if (lc == 0)
				return 0;
			continue;
		}

		if ((lc == FKS_FNAME_C('/') && rc == FKS_FNAME_C('\\')) || (lc == FKS_FNAME_C('\\') && rc == FKS_FNAME_C('/')))
			continue;

		return n;
	}
	return 0;
 #else
	int i;
	FKS_ASSERT( l != 0 && r != 0 );
  #if 1
	return FKS_FNAME_N_CMP(l, r, len);
  #else
	char*	orig = setlocale(LC_CTYPE, s_fks_fname_locale_ctype);
	i = FKS_FNAME_CMP(l, r);
	setlocale(orig);
	return i;
  #endif
 #endif
}


/** �� len ��蒷��������ŁAlen�����ڂ� ���l��̓r���������ꍇ�Alen�𒴂���strtol���Ă��܂�����
 *	   �Ӑ}�������ʂɂȂ�Ȃ��ꍇ������B�̂ŁAfnameNDigitCmp�͌��J�����T�u���[�`���Ƃ���.
 */
FKS_STATIC_DECL (int)
fks_fnameNDigitCmp(const FKS_FNAME_CHAR* l, const FKS_FNAME_CHAR* r, FKS_FNAME_SIZE len) FKS_NOEXCEPT
{
	const FKS_FNAME_CHAR* e = l + len;
	FKS_ASSERT( l != 0 && r != 0 );
	if (e < l)
		e = (const FKS_FNAME_CHAR*)-1;
	while (l < e) {
		intmax_t	n;
		unsigned	lc;
		unsigned	rc;

		FKS_FNAME_GET_C(lc, l);
		FKS_FNAME_GET_C(rc, r);

		if (lc <= 0x80 && FKS_FNAME_IS_DIGIT(lc) && rc <= 0x80 && FKS_FNAME_IS_DIGIT(rc)) {
			intmax_t	lv = FKS_FNAME_STRTOL(l - 1, (FKS_FNAME_CHAR**)&l, 10);
			intmax_t	rv = FKS_FNAME_STRTOL(r - 1, (FKS_FNAME_CHAR**)&r, 10);
			n = lv - rv;
			if (n == 0)
				continue;
			return (n < 0) ? -1 : 1;
		}

	 #ifdef FKS_FNAME_WINDOS
		lc = FKS_FNAME_TO_LOWER(lc);
		rc = FKS_FNAME_TO_LOWER(rc);
	 #endif

		n  = (intmax_t)(lc - rc);
		if (n == 0) {
			if (lc == 0)
				return 0;
			continue;
		}

	  #ifdef FKS_FNAME_WINDOS
		if ((lc == FKS_FNAME_C('/') && rc == FKS_FNAME_C('\\')) || (lc == FKS_FNAME_C('\\') && rc == FKS_FNAME_C('/')))
			continue;
	  #endif
		return (n < 0) ? -1 : 1;
	}
	return 0;
}


/** �t�@�C�����̑召��r. ���l���������ꍇ�A�����Ⴂ�̐��l���m�̑召�𔽉f
*	win/dos�n�͑召���ꎋ. �f�B���N�g���Z�p���[�^ \ / �����ꎋ.
*	�ȊO�͒P���ɕ������r.
*/
FKS_LIB_DECL(int)
fks_fnameDigitCmp(const FKS_FNAME_CHAR* l, const FKS_FNAME_CHAR* r) FKS_NOEXCEPT
{
	return fks_fnameNDigitCmp(l, r, (FKS_FNAME_SIZE)-1);
}


/** fname��baseName�Ŏn�܂��Ă���΁Afname�̗]���̐擪�̃A�h���X��Ԃ�.
 *	�}�b�`���Ă��Ȃ����NULL��Ԃ�.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameEquLong(FKS_FNAME_const_CHAR* fname, const FKS_FNAME_CHAR* baseName) FKS_NOEXCEPT
{
	FKS_FNAME_SIZE l;
	FKS_ASSERT(fname && baseName);
	l = fks_fnameLen(baseName);
	if (l == 0)
		return (FKS_FNAME_CHAR*)fname;
	return (fks_fnameNCmp(fname, baseName, l) == 0) ? (FKS_FNAME_CHAR*)fname+l : 0;
}


/** �g���q�̈ʒu��Ԃ�. '.'�͊܂�. �Ȃ���Ε�����̍Ō��Ԃ�.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameExt(FKS_FNAME_const_CHAR* name) FKS_NOEXCEPT
{
	const FKS_FNAME_CHAR *p;
	FKS_ARG_PTR_ASSERT(1, name);
	name = fks_fnameBaseName(name);
	p	 = FKS_FNAME_R_STR(name, FKS_FNAME_C('.'));
	if (p)
		return (FKS_FNAME_CHAR*)(p);

	return (FKS_FNAME_CHAR*)name + fks_fnameLen(name);
}


/** �t�@�C���p�X�����̊g���q���폜����.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameDelExt(FKS_FNAME_CHAR buf[]) FKS_NOEXCEPT
{
	FKS_FNAME_CHAR *t;
	FKS_FNAME_CHAR *p;
	FKS_ARG_PTR_ASSERT(1, buf);
	t = fks_fnameBaseName(buf);
	p = FKS_FNAME_R_STR(t, FKS_FNAME_C('.'));
	if (p == 0)
		p = t + fks_fnameLen(t);
	*p = 0;
	return buf;
}


/** �t�@�C���p�X�����̊g���q�������������̎擾.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameGetNoExt(FKS_FNAME_CHAR dst[], FKS_FNAME_SIZE size, const FKS_FNAME_CHAR *src) FKS_NOEXCEPT
{
	const FKS_FNAME_CHAR *s;
	const FKS_FNAME_CHAR *e;
	FKS_FNAME_SIZE		  l = 0;
	FKS_ARG_PTR_ASSERT(1, dst);
	FKS_RANGE_UINTPTR_ASSERT(2, 2, (FKS_FNAME_SIZE)-1);
	FKS_ARG_PTR_ASSERT(3, src);
	//if (dst == 0 || size == 0 || src == 0) return 0;
	s = fks_fnameBaseName(src);
	e = FKS_FNAME_R_STR(s, FKS_FNAME_C('.'));
	if (e == 0)
		e = s + fks_fnameLen(s);
	//l = e - src + 1;
	l = e - src;
	if (l > size)
		l = size;
	fks_fnameCpy(dst, l, src);
	return dst;
}


/** �g���q���Aext �ɕύX����. dst == src �ł��悢.
 *	ext = NULL or "" �̂Ƃ��� �g���q�폜.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameSetExt(FKS_FNAME_CHAR dst[], FKS_FNAME_SIZE size, const FKS_FNAME_CHAR* src, const FKS_FNAME_CHAR *ext) FKS_NOEXCEPT
{
	FKS_ASSERT(dst != 0 && size > 0 && src != 0);
	fks_fnameGetNoExt(dst,	size, src);
	if (ext && ext[0]) {
		if (ext[0] != FKS_FNAME_C('.'))
			fks_fnameCat(dst, size, FKS_FNAME_C("."));
		fks_fnameCat(dst, size, ext);
	}
	return dst;
}


/** �g���q���Ȃ��ꍇ�A�g���q��ǉ�����.(����Ή������Ȃ�). dst == src �ł��悢.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameSetDefaultExt(FKS_FNAME_CHAR dst[], FKS_FNAME_SIZE size, const FKS_FNAME_CHAR* src, const FKS_FNAME_CHAR *ext) FKS_NOEXCEPT
{
	FKS_FNAME_CHAR* p;
	FKS_ASSERT(dst != 0 && size > 0 && src != 0);

	fks_fnameCpy(dst, size, src);
	if (ext == 0)
		return dst;
	p = fks_fnameBaseName(dst);
	p = FKS_FNAME_R_STR(p, FKS_FNAME_C('.'));
	if (p) {
		if (p[1])
			return dst;
		*p = 0;
	}
	if (ext[0]) {
		if (ext[0] != FKS_FNAME_C('.'))
			fks_fnameCat(dst, size, FKS_FNAME_C("."));
		fks_fnameCat(dst, size, ext);
	}
	return dst;
}


/** ������̍Ō�� \ �� / ������΂��̈ʒu��Ԃ��A�Ȃ����NULL��Ԃ�.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameCheckLastSep(FKS_FNAME_const_CHAR* dir) FKS_NOEXCEPT
{
	FKS_FNAME_SIZE l = fks_fnameLen(dir);
	if (l == 0) return 0;
	return fks_fnameCheckPosSep(dir, l	- 1);
}


/** ������̎w��ʒu�� \ �� / ������΂��̈ʒu��Ԃ��A�Ȃ����NULL��Ԃ�.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameCheckPosSep(FKS_FNAME_const_CHAR* dir, int ofs) FKS_NOEXCEPT
{
	FKS_ASSERT(dir != 0);
	if (dir) {
		const FKS_FNAME_CHAR*		s	= dir;
		if (ofs >= 0) {
			const FKS_FNAME_CHAR*	p	= s + ofs;
			if (*p == FKS_FNAME_C('/'))
				return (FKS_FNAME_CHAR *)p;
		  #if (defined FKS_FNAME_WINDOS)
			else if (*p == FKS_FNAME_C('\\')) {
			  #ifdef FKS_FNAME_WCS_COMPILE
				return (FKS_FNAME_CHAR *)p;
			  #else 	// adjust_size�̌��ʂ�ofs�����ɂȂ��Ă���*p�̓}���`�o�C�g�����̈ꕔ.
				if (fks_fnameAdjustSize(s, ofs) == ofs)
					return (FKS_FNAME_CHAR *)p;
			  #endif
			}
		  #endif
		}
	}
	return NULL;
}


/** ������̍Ō�� \ �� / ������΍폜
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameDelLastSep(FKS_FNAME_CHAR dir[]) FKS_NOEXCEPT
{
	FKS_FNAME_CHAR* p = fks_fnameCheckLastSep(dir);
	if (p)
		*p = 0;
	return dir;
}


/** ������̍Ō�� �f�B���N�g���Z�p���[�^�������Ȃ���Βǉ�.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameAddSep(FKS_FNAME_CHAR dir[], FKS_FNAME_SIZE size) FKS_NOEXCEPT
{
	FKS_FNAME_CHAR* e = dir + size;
	FKS_FNAME_CHAR* p = fks_fnameCheckLastSep(dir);
	if (p == 0) {
		p = dir + fks_fnameLen(dir);
		if (p+1 < e) {
			*p++ = FKS_FNAME_SEP_CHR;
			*p = 0;
		}
	}
	return dir;
}


/** �h���C�u�������Ă��邩.
 */
FKS_LIB_DECL (int)
fks_fnameHasDrive(const FKS_FNAME_CHAR* path) FKS_NOEXCEPT
{
  #if 1 // �擪��"������:"���h���C�u���Ƃ݂Ȃ�.
	const FKS_FNAME_CHAR* s = path;
	if (s == 0)
		return 0;
	if (*s && *s != FKS_FNAME_C(':')) {
		while (*s && !fks_fnameIsSep(*s)) {
			if (*s == FKS_FNAME_C(':'))
				return 1;
			++s;
		}
	}
	return 0;
  #else // �ꎚ�h���C�u���̂ݑΉ�����ꍇ.
   #if defined FKS_FNAME_WINDOS
	if (path == 0)
		return 0;
	return (path[0] && path[1] == FKS_FNAME_C(':'));
   #else
	return 0;
   #endif
  #endif
}


/** �h���C�u��������΂�����X�L�b�v�����|�C���^��Ԃ�.
 *	 �� c:�������łȂ�http:���X�L�b�v���邽�� "������:" ���X�L�b�v.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameSkipDrive(FKS_FNAME_const_CHAR* path) FKS_NOEXCEPT
{
  #if 1
	const FKS_FNAME_CHAR* s = path;
	if (*s && *s != FKS_FNAME_C(':')) {
		while (*s && !fks_fnameIsSep(*s)) {
			if (*s == FKS_FNAME_C(':'))
				return (FKS_FNAME_CHAR*)s+1;
			++s;
		}
	}
 #else	// 1���h���C�u���݂̂̑Ή��Ȃ炱����.
  #if defined FKS_FNAME_WINDOS
	if (path[0] && path[1] == FKS_FNAME_C(':')) 	// �h���C�u���t��������
		return (FKS_FNAME_CHAR*) path + 2;
	#endif
 #endif
	return (FKS_FNAME_CHAR*)path;
}


/** ��΃p�X���ۂ�(�h���C�u���̗L���͊֌W�Ȃ�)
 */
FKS_LIB_DECL (int)
fks_fnameIsAbs(const FKS_FNAME_CHAR* path) FKS_NOEXCEPT 
{
	if (path == 0)
		return 0;
	path = fks_fnameSkipDrive(path);
	return fks_fnameIsSep(path[0]);
}


#ifndef _WIN32	// win �ł� ascii �ȊO�̕����ł��t�@�C�����̏������啶�����肵�ē��ꎋ���Ă�̂ł��ꂾ���ł͑ʖڂ�����.
/** �S�p�Q�o�C�g�ڂ��l������ strlwr
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameToLowerN(FKS_FNAME_CHAR name[], size_t n) FKS_NOEXCEPT
{
	FKS_FNAME_CHAR *p = name;
	FKS_FNAME_CHAR *e = p + n;
	FKS_ASSERT(name != NULL);

	while (p < e) {
		unsigned c = *p;
		*p = FKS_FNAME_TO_LOWER(c);
		p  = FKS_FNAME_CHARNEXT(p);
	}
	return name;
}
#endif

FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameToLower(FKS_FNAME_CHAR name[]) FKS_NOEXCEPT
{
 #if _WIN32
	return FKS_STRLWR(name);
 #else
	return fks_fnameToLowerN(name, fks_fnameLen(name));
 #endif
}


#ifndef _WIN32	// win �ł� ascii �ȊO�̕����ł��t�@�C�����̏������啶�����肵�ē��ꎋ���Ă�̂ł��ꂾ���ł͑ʖڂ�����.
/** �S�p�Q�o�C�g�ڂ��l������ strupr
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameToUpperN(FKS_FNAME_CHAR name[], size_t n) FKS_NOEXCEPT
{
	FKS_FNAME_CHAR *p = name;
	FKS_FNAME_CHAR *e = p + n;
	FKS_ASSERT(name != NULL);
	while (p < e) {
		unsigned c = *p;
		*p = FKS_FNAME_TO_UPPER(c);
		p  = FKS_FNAME_CHARNEXT(p);
	}
	return name;
}
#endif

FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameToUpper(FKS_FNAME_CHAR name[]) FKS_NOEXCEPT
{
 #ifdef _WIN32
	return FKS_STRUPR(name);
 #else
	return fks_fnameToUpperN(name, fks_fnameLen(name));
 #endif
}


/** �t�@�C���p�X�����̃f�B���N�g���Ɗg���q���������t�@�C�����̎擾.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameGetBaseNameNoExt(FKS_FNAME_CHAR dst[], FKS_FNAME_SIZE size, const FKS_FNAME_CHAR *src) FKS_NOEXCEPT
{
	const FKS_FNAME_CHAR *s;
	const FKS_FNAME_CHAR *e;
	FKS_FNAME_SIZE		  l = 0;
	FKS_ASSERT(dst != 0 && size > 1 && src != 0);
	//if (dst == 0 || size == 0 || src == 0) return 0;
	s = fks_fnameBaseName(src);
	e = FKS_FNAME_R_STR(s, FKS_FNAME_C('.'));
	if (e == 0)
		e = s + fks_fnameLen(s);
	l = e - s; // +1;
	if (l > size)
		l = size;
	fks_fnameCpy(dst, l, s);
	return dst;
}


/** �f�B���N�g��������Ԃ�. �Ō�̃f�B���N�g���Z�p���[�^�͊O��.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameGetDir(FKS_FNAME_CHAR dir[], FKS_FNAME_SIZE size, const FKS_FNAME_CHAR *name) FKS_NOEXCEPT
{
	const FKS_FNAME_CHAR*	p;
	size_t					l;

	FKS_ASSERT(dir	!= 0 && size > 0 && name != 0);

	p = fks_fnameBaseName(name);
	l = p - name; // +1;
	if (l > size)
		l = size;
	if (l && dir != name)
		fks_fnameCpy(dir, l, name);
	dir[l] = 0;
	if (l > 0)
		fks_fnameDelLastSep(dir);
	return dir;
}


/** �h���C�u���������擾. :��. �� file:���̑Ώ��̂���"������:"���h���C�u����
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameGetDrive(FKS_FNAME_CHAR drive[], FKS_FNAME_SIZE size, const FKS_FNAME_CHAR *name) FKS_NOEXCEPT
{
	const FKS_FNAME_CHAR*	s;
	FKS_FNAME_SIZE			l;
	FKS_ASSERT(drive && size > 0 && name);
	drive[0] = 0;
	s = fks_fnameSkipDrive(name);
	l = s - name;
	if (l > 0) {
		//++l;
		if (l > size)
			l = size;
		fks_fnameCpy(drive, l,	name);
	}
	return drive;
}


/** �h���C�u���ƃ��[�g�w�蕔�����擾.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameGetDriveRoot(FKS_FNAME_CHAR dr[], FKS_FNAME_SIZE size, const FKS_FNAME_CHAR *name) FKS_NOEXCEPT
{
	const FKS_FNAME_CHAR*	s;
	FKS_FNAME_SIZE			l;
	FKS_ASSERT(dr && size > 0 && name);
	dr[0] = 0;
	s = fks_fnameSkipDriveRoot(name);
	l = s - name;
	if (l > 0) {
		//++l;
		if (l > size)
			l = size;
		fks_fnameCpy(dr, l, name);
	}
	return dr;
}


/** ������擪�̃h���C�u��,���[�g�w����X�L�b�v�����|�C���^��Ԃ�.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameSkipDriveRoot(FKS_FNAME_const_CHAR* path) FKS_NOEXCEPT
{
	FKS_FNAME_CHAR* p = fks_fnameSkipDrive(path);
	while (fks_fnameIsSep(*p))
		++p;
	return p;
}


/** filePath���� \ �� / �ɒu��.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameBackslashToSlash(FKS_FNAME_CHAR filePath[]) FKS_NOEXCEPT
{
	FKS_FNAME_CHAR *p = filePath;
	FKS_ASSERT(filePath != NULL);
	while (*p != FKS_FNAME_C('\0')) {
		if (*p == FKS_FNAME_C('\\')) {
			*p = FKS_FNAME_C('/');
		}
		p = FKS_FNAME_CHARNEXT(p);
	}
	return filePath;
}


/** filePath���� / �� \ �ɒu��.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameSlashToBackslash(FKS_FNAME_CHAR filePath[]) FKS_NOEXCEPT
{
	FKS_FNAME_CHAR *p;
	FKS_ASSERT(filePath != NULL);
	for (p = filePath; *p; ++p) {
		if (*p == FKS_FNAME_C('/'))
			*p = FKS_FNAME_C('\\');
	}
	return filePath;
}


FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameFullpath(FKS_FNAME_CHAR dst[], FKS_FNAME_SIZE size, const FKS_FNAME_CHAR* path, const FKS_FNAME_CHAR* currentDir)  FKS_NOEXCEPT
{
  #if defined FKS_FNAME_WINDOS
	return fks_fnameFullpathBS(dst, size, path, currentDir);
  #else
	return fks_fnameFullpathSL(dst, size, path, currentDir);
  #endif
}


/** �t���p�X����. �f�B���N�g���Z�p���[�^��\\�ɂ��ĕԂ��o�[�W����.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameFullpathBS(FKS_FNAME_CHAR	dst[], FKS_FNAME_SIZE size, const FKS_FNAME_CHAR* path, const FKS_FNAME_CHAR* currentDir) FKS_NOEXCEPT
{
	fks_fnameFullpathSL(dst, size,	path, currentDir);
	fks_fnameSlashToBackslash(dst);
	return dst;
}


/** �t���p�X����. �����񑀍�̂�. �J�����g�p�X�͈����œn��.
 *	currentDir �͐�΃p�X�ł��邱��. �����łȂ��ꍇ�̋����͕s��.
 *	'\'�����΍�ŁA�Z�p���[�^��'/'�ɒu�����Ă���.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameFullpathSL(FKS_FNAME_CHAR	dst[], FKS_FNAME_SIZE size, const FKS_FNAME_CHAR* path, const FKS_FNAME_CHAR* currentDir) FKS_NOEXCEPT
{
	FKS_FNAME_CHAR* 	wk;
	FKS_FNAME_SIZE		wkSz;

	FKS_ASSERT(dst != 0 && size > 2 && path != 0);
	if (dst == 0 || size <= 2 || path == 0)
		return 0;
	if (currentDir == 0)
		currentDir = FKS_FNAME_C("/");	// DIRSEP_STR;
	FKS_ASSERT(fks_fnameIsAbs(currentDir));

  #ifdef FKS_FNAME_USE_ALLOCA
	// �������ݐ�T�C�Y���\���łȂ���΍�Ɨp���������m��.
	{
		FKS_FNAME_SIZE pl = fks_fnameLen(path);
		FKS_FNAME_SIZE cl = fks_fnameLen(currentDir);
		wkSz = pl + cl + 4;
		if (wkSz >= size) { 	// dst�T�C�Y�����A���������Ȃ烏�[�N��p��.
			FKS_ASSERT( wkSz <= FKS_FNAME_MAX_URL * sizeof(FKS_FNAME_CHAR) );
			wk = (FKS_FNAME_CHAR*)fks_alloca(wkSz*sizeof(FKS_FNAME_CHAR));
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
		unsigned hasDrive = fks_fnameHasDrive(path);
		unsigned isAbs	  = fks_fnameIsAbs(path);
		wk[0] = 0;
		if (isAbs && hasDrive) {	// �h���C�u�t����΃p�X�Ȃ�A���̂܂�.
			fks_fnameCpy(wk, wkSz,	path);
		} else if (isAbs) {
			if (fks_fnameHasDrive(currentDir))	// ��΃p�X�����ǃh���C�u���Ȃ��ꍇ��currentDir����h���C�u������������.
				fks_fnameGetDrive(wk, wkSz, currentDir);
			fks_fnameCat(wk, wkSz,	path);
		} else {
			if (hasDrive) { 		// �h���C�u�t�����΃p�X�ŁA
				if (!fks_fnameHasDrive(currentDir)) // �J�����g���Ƀh���C�u���Ȃ����
					fks_fnameGetDrive(wk, wkSz, path);	// path�̃h���C�u����ݒ�. �������΃J�����g���̃h���C�u���ɂȂ�.
			}
			fks_fnameCat(wk, wkSz,	currentDir);
			fks_fnameAddSep(wk, wkSz);
			fks_fnameCat(wk, wkSz,	fks_fnameSkipDrive(path));
		}
	}

  #if defined FKS_FNAME_WINDOS
	// �������ȒP�ɂ��邽�߁A�p�X�̋�؂����U / �ɕϊ�.
	fks_fnameBackslashToSlash(wk);
  #endif

	// "." �� ".." ����菜��.
	{
		// ���̎��_��wk�͕K����΃p�X�ɂȂ��Ă���.(currentDir���ᔽ���Ă��ꍇ�̋����͕s�舵��).
		FKS_FNAME_CHAR* 	s	  = fks_fnameSkipDrive(wk); // �h���C�u���͘M��Ȃ��̂ŃX�L�b�v.
		FKS_FNAME_CHAR* 	d	  = s;
		FKS_FNAME_CHAR* 	top   = d;
		unsigned			first = 1;
		while (*s) {
			unsigned c = *s++;
			if (c == FKS_FNAME_C('/')) {
				if (first) {	// ����� / �� "//" "///" ������... ���Ƃ�*d++=c����̂ł����ł�2��܂�.
					unsigned i;
					for (i = 0; i < 2 && *s == FKS_FNAME_C('/'); ++i)
						*d++ = *s++;
				}
				first = 0;
				// '/'�̘A���͈��'/'����.
			  RETRY:
				while (*s == FKS_FNAME_C('/'))
					++s;
				if (*s == FKS_FNAME_C('.')) {
					if (s[1] == 0) {					// "." �݂͖̂���.
						s += 1;
						goto RETRY;
					} else if (s[1] == FKS_FNAME_C('/')) {		// "./" �͖���.
						s += 2;
						goto RETRY;
					} else if (s[1] == FKS_FNAME_C('.') && (s[2] == FKS_FNAME_C('/') || s[2] == 0)) {	// "../" ".." �̂Ƃ�.
						s += 2 + (s[2] != 0);
						while (d > top && *--d != FKS_FNAME_C('/'))	// �o�͐�̃f�B���N�g���K�w���P���炷.
							;
						goto RETRY;
					}
				}
			}
			*d++ = c;
		}
		*d = 0;
	}

  #ifdef FKS_FNAME_USE_ALLOCA
	if (wk != dst)	// ���[�N��alloca���Ă��̂Ȃ�A�R�s�[.
		fks_fnameCpy(dst, size, wk);
  #endif

	return dst;
}


FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameRelativePath(FKS_FNAME_CHAR dst[], FKS_FNAME_SIZE size, const FKS_FNAME_CHAR* path, const FKS_FNAME_CHAR* currentDir)  FKS_NOEXCEPT
{
  #if defined FKS_FNAME_WINDOS
	return fks_fnameRelativePathBS(dst, size, path, currentDir);
  #else
	return fks_fnameRelativePathSL(dst, size, path, currentDir);
  #endif
}

/** ���΃p�X����. �f�B���N�g���Z�p���[�^��\\�ɂ��ĕԂ��o�[�W����.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameRelativePathBS(FKS_FNAME_CHAR	dst[], FKS_FNAME_SIZE size, const FKS_FNAME_CHAR* path, const FKS_FNAME_CHAR* currentDir) FKS_NOEXCEPT
{
	fks_fnameRelativePathSL(dst, size,	path, currentDir);
	fks_fnameSlashToBackslash(dst);
	return dst;
}


/** currentDir����̑��΃p�X����.
 *	currentDir �͐�΃p�X�ł��邱��. �����łȂ��ꍇ�̋����͕s��.
 *	'\'�����΍�ŁA�Z�p���[�^��'/'�ɒu�����Ă���.
 *	�J�����g�f�B���N�g������n�܂�ꍇ�A"./"�͂��Ȃ�.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameRelativePathSL(FKS_FNAME_CHAR dst[], FKS_FNAME_SIZE size, const FKS_FNAME_CHAR* path, const FKS_FNAME_CHAR* currentDir) FKS_NOEXCEPT
{
	enum { CHECK_MAX_PATH = sizeof(char[FKS_FNAME_MAX_PATH >= 16 ? 1 : -1]) };	// �R���p�C�����̃T�C�Y�`�F�b�N.
	enum { CHECK_MAX_URL  = sizeof(char[FKS_FNAME_MAX_URL  >= 16 ? 1 : -1]) };	// �R���p�C�����̃T�C�Y�`�F�b�N.
	FKS_FNAME_CHAR		curDir	[ FKS_FNAME_MAX_URL + 1 ];
	FKS_FNAME_CHAR		fullName[ FKS_FNAME_MAX_URL + 1 ];
	FKS_FNAME_CHAR* 	cp;
	FKS_FNAME_CHAR* 	cpSav;
	FKS_FNAME_CHAR* 	fp;
	FKS_FNAME_SIZE		cl;
	FKS_FNAME_SIZE		fl;

	FKS_ASSERT(dst != 0 && size > 2 && path != 0);
	if (dst == 0 || size <= 2 || path == 0)
		return 0;

	// �܂��A�J�����g�f�B���N�g�����t���p�X��(/) & �Ō��/��t��.
	FKS_ASSERT(fks_fnameIsAbs(currentDir));
	fks_fnameFullpathSL(curDir, FKS_FNAME_MAX_URL,	currentDir, FKS_FNAME_C("/"));
	cp = fks_fnameCheckLastSep(curDir);
	if (cp == 0) {
		cp = curDir + fks_fnameLen(curDir);
		if (cp+1 < curDir+FKS_FNAME_MAX_URL) {
			*cp++ = FKS_FNAME_C('/');
			*cp = 0;
		}
	}

	// �Ώۂ� path ���t���p�X��. \\�͖ʓ|�Ȃ̂�/���������.
	fks_fnameFullpathSL(fullName, FKS_FNAME_MAX_URL, path,	curDir);

	// �}�b�`���Ă���f�B���N�g���������X�L�b�v.
	cp	  = fks_fnameSkipDriveRoot(curDir);
	fp	  = fks_fnameSkipDriveRoot(fullName);

	// �h���C�u��������Ă����瑊�΃p�X�����Ȃ�.
	cl	  = cp - curDir;
	fl	  = fp - fullName;
	if (cl != fl || fks_fnameNCmp(curDir,	fullName, fl) != 0) {
		fks_fnameCpy(dst, size, fullName);
		return dst;
	}

	// �����������`�F�b�N.
	cpSav = cp;
	while (*cp && *fp) {
		unsigned cc;
		unsigned fc;
		FKS_FNAME_GET_C(cc, cp);
		FKS_FNAME_GET_C(fc, fp);
		if (cc != fc)
			break;
		if (*cp == FKS_FNAME_C('/'))
			cpSav = (FKS_FNAME_CHAR*)cp + 1;
	}
	fp		= fp - (cp - cpSav);
	cp		= cpSav;

	// �J�����g�ʒu�����ւ̈ړ�����../�𐶐�.
	{
		FKS_FNAME_CHAR* d = dst;
		FKS_FNAME_CHAR* e = dst + size - 1;
		while (*cp) {
			if (*cp == FKS_FNAME_C('/')) {
				if (d < e) *d++ = FKS_FNAME_C('.');
				if (d < e) *d++ = FKS_FNAME_C('.');
				if (d < e) *d++ = FKS_FNAME_C('/');
			}
			++cp;
		}
		*d = FKS_FNAME_C('\0');
	}

	// �J�����g�ʒu�ȉ��̕������R�s�[
	fks_fnameCat(dst, size, fp);

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
fks_fnameMatchWildCard(const FKS_FNAME_CHAR* ptn, const FKS_FNAME_CHAR* tgt) FKS_NOEXCEPT
{
	unsigned				tc;
	const FKS_FNAME_CHAR*	tgt2 = tgt;
	FKS_FNAME_GET_C(tc, tgt2);	// 1���擾& tgt�|�C���^�X�V.
	switch (*ptn) {
	case FKS_FNAME_C('\0'):
		return tc == FKS_FNAME_C('\0');

  #if defined FKS_FNAME_WINDOWS
	case FKS_FNAME_C('\\'):
  #endif
	case FKS_FNAME_C('/'):
		return fks_fnameIsSep(tc) && fks_fnameMatchWildCard(ptn+1, tgt2);

	case FKS_FNAME_C('?'):
		return tc && !fks_fnameIsSep(tc) && fks_fnameMatchWildCard( ptn+1, tgt2 );

	case FKS_FNAME_C('*'):
		if (ptn[1] == FKS_FNAME_C('*')) // ** �Ȃ�Z�p���[�^�ɂ��}�b�`.
			return fks_fnameMatchWildCard(ptn+2, tgt) || (tc && fks_fnameMatchWildCard( ptn, tgt2	));
		return fks_fnameMatchWildCard(ptn+1, tgt) || (tc && !fks_fnameIsSep(tc) && fks_fnameMatchWildCard( ptn, tgt2	));

	default:
		{
			unsigned pc;
			FKS_FNAME_GET_C(pc, ptn);	// 1���擾& ptn�|�C���^�X�V.
			return (pc == tc) && fks_fnameMatchWildCard(ptn, tgt2);
		}
	}
}


/** �R�}���h���C��������A;��؂�̕����p�X�w����A��������̂Ɏg��.
 *	""��win�R�}���h���C���ɂ��킹������.
 *	sepChr�ŋ�؂�ꂽ������(�t�@�C����)���擾. 0x20�ȊO�̋󔒂͖�����0x20����.
 *	@return �X�L�����X�V��̃A�h���X��Ԃ��Bstr��EOS��������NULL��Ԃ�.
 */
FKS_LIB_DECL (FKS_FNAME_CHAR*)
fks_fnameScanArgStr(FKS_FNAME_CHAR arg[], FKS_FNAME_SIZE argSz, const FKS_FNAME_CHAR *str, unsigned sepChr) FKS_NOEXCEPT
{
  #ifdef FKS_FNAME_WCS_COMPILE
	const FKS_FNAME_CHAR*	s = str;
  #else
	const unsigned char*	s = (const unsigned char*)str;
  #endif
	FKS_FNAME_CHAR* 		d = arg;
	FKS_FNAME_CHAR* 		e = d + argSz;
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
		if (c == FKS_FNAME_C('"')) {
			f ^= 1; 						// "�̑΂̊Ԃ͋󔒂��t�@�C�����ɋ���.���߂̃t���O.

			// ������ƋC�����������AWin(XP)��cmd.exe�̋����ɍ��킹�Ă݂�.
			// (�ق�Ƃɂ����Ă邩�A�\���ɂ͒��ׂĂȂ�)
			if (*s == FKS_FNAME_C('"') && f == 0)	// ��"�̒���ɂ����"������΁A����͂��̂܂ܕ\������.
				++s;
			else
				continue;					// �ʏ�� " �͏Ȃ��Ă��܂�.
		}
		if ((c > 0 && c < 0x20) || c == 0x7f)
			c = FKS_FNAME_C(' ');
		if (d < e)
			*d++ = (FKS_FNAME_CHAR)c;
	} while (c >= 0x20 && c != 0x7f && (f || (c != sepChr)));
	*--d  = FKS_FNAME_C('\0');
	--s;
	return (FKS_FNAME_CHAR *)s;
}


// ----------------------------------
// win �ȊO�ŁAsjis�Ή�����ꍇ�̏���.

#if defined(FKS_USE_FNAME_MBC) && !defined(_WIN32) && !defined(FKS_FNAME_WCS_COMPILE)

FKS_PRIV_LIB_DECL(int)	fks_fnameMbcInit(void);

static int	  s_fks_fname_shift_char_mode =	0;

FKS_PRIV_LIB_DECL(int)	fks_fnameIsZenkaku1(unsigned c) FKS_NOEXCEPT {
	if (s_fks_fname_shift_char_mode	== 0)
		s_fks_fname_shift_char_mode	= fks_fnameMbcInit();
	switch (s_fks_fname_shift_char_mode) {
	case 1 /* sjis */: return ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC));
	case 2 /* GBK  */: return  (c >= 0x81 && c <= 0xFE);
	case 3 /* BIG5 */: return ((c >= 0xA1 && c <= 0xC6) || (c >= 0xC9 && c <= 0xF9));
	default:		   return -1;
	}
}

/** �Ƃ肠�����A0x5c�֌W�̑Ώ��p.
 */
FKS_PRIV_LIB_DECL(int)	fks_fnameMbcInit(void) FKS_NOEXCEPT 
{
	const char* 	lang = getenv("LANG");
	const char* 	p;
	if (lang == 0)
		return -1;
	//s_fks_fname_locale_ctype = strdup(lang);
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

#endif	// !defined(_WIN32) && !defined(FKS_FNAME_WCS_COMPILE)


#if (defined __cplusplus) && !(defined FKS_FNAME_WCS_COMPILE)
}
#endif
