/*
 *  @file   fks_mbc.h
 *  @brief  Multi Byte Character lib.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_MBC_H_INCLUDED
#define FKS_MBC_H_INCLUDED

#include <fks_common.h>
#include <fks_types.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FKS_WIN32
typedef int		fks_codepage_t;
enum { FKS_CP_UTF8 = 65001, FKS_CP_UTF16LE=1200, FKS_CP_UTF16BE=1201, FKS_CP_UTF32LE=12000, FKS_CP_UTF32BE=12001
	 , FKS_CP_1BYTE = 437/*kari*/, FKS_CP_SJIS = 932, FKS_CP_EUCJP = 20932, FKS_CP_EUCJP1 = 51932 };
#else	// kari
typedef uintptr_t	fks_codepage_t;
enum { FKS_CP_UTF8 = 65001, FKS_CP_UTF16LE=1200, FKS_CP_UTF16BE=1201, FKS_CP_UTF32LE=12000, FKS_CP_UTF32BE=12001, FKS_CP_SJIS = 932, FKS_CP_EUCJP = 20932, FKS_CP_EUCJP1 = 51932 };
#endif

typedef struct Fks_MbcEnv {
	fks_codepage_t	cp;
    unsigned (*isLead)(unsigned c);
    unsigned (*chkC)(unsigned c);
    unsigned (*getC)(char const** str, char const* e);
    unsigned (*peekC)(char const* str, char const* e);
	char*	 (*charNext)(char const* str, char const* e);
    char*    (*setC)(char* dst, char* e, unsigned c);
    unsigned (*len1)(char const* pChr, char const* e);
    unsigned (*chrLen)(unsigned chr);
    unsigned (*chrWidth)(unsigned chr);
    unsigned (*chrWidthJp)(unsigned chr);
	size_t   (*adjustSize)(char const* str, size_t size);
	size_t   (*sizeToChrs)(char const* str, size_t size);
	size_t   (*chrsToSize)(char const* str, size_t chrs);
	int 	 (*cmp)(char const* lp, char const* rp);
} Fks_MbcEnv;

extern Fks_MbcEnv const* const	fks_mbc_utf8;
extern Fks_MbcEnv const* const	fks_mbc_utf16le;
extern Fks_MbcEnv const* const	fks_mbc_utf16be;
extern Fks_MbcEnv const* const	fks_mbc_utf32le;
extern Fks_MbcEnv const* const	fks_mbc_utf32be;
extern Fks_MbcEnv const* const  fks_mbc_asc;
#ifdef FKS_WIN32
extern Fks_MbcEnv const* const	fks_mbc_dbc;
#endif
extern Fks_MbcEnv const* const	fks_mbc_sjis;
extern Fks_MbcEnv const* const	fks_mbc_eucJp;

size_t  fks_mbcSizeToWidth(Fks_MbcEnv const* mbc, char const* str, size_t size);
size_t  fks_mbcChrsToWidth(Fks_MbcEnv const* mbc, char const* str, size_t chrs);
size_t  fks_mbcWidthToSize(Fks_MbcEnv const* mbc, char const* str, size_t width);
size_t  fks_mbcWidthToChrs(Fks_MbcEnv const* mbc, char const* str, size_t width);
size_t  fks_mbcCpy(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, char const* src);
size_t  fks_mbcLCpy(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, char const* src, size_t l);
char*   fks_mbcCat(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, char const* src);
char*   fks_mbcCpyNC(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, char const* src, size_t nc);
char*   fks_mbcCatNC(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, char const* src, size_t nc);
char*   fks_mbcCpyWidth(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, char const* src, size_t width);
char*   fks_mbcCatWidth(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, char const* src, size_t width);

size_t  fks_mbc_unicodeConv(Fks_MbcEnv const* dstMbc, char dst[], size_t dstSz, Fks_MbcEnv const* srcMbc, char const* src, size_t srcSz);
Fks_MbcEnv const* fks_mbc_checkUnicodeBOM(char const* src, size_t len);
int 	fks_mbc_checkUnicodeBOMi(char const* src, size_t len);	///< 0:non 1:utf8-BOM 2:utf16le-BOM 3:utf16be-BOM 4:utf32le-BOM 5:utf32be-BOM
int 	fks_mbc_checkUTF8(char const* src, size_t len);	///< 0:not  1:ascii(<=7f) >=2:utf8

#ifdef FKS_WIN32
size_t   fks_mbc_unicodeDbcConv(Fks_MbcEnv const* dstMbc, char dst[], size_t dstSz, Fks_MbcEnv const* srcMbc, char const* src, size_t srcSz);
Fks_MbcEnv const* fks_mbc_makeDBC(Fks_MbcEnv* mbcEnv, fks_codepage_t cp);
//size_t   fks_mbc_dbcFromUnicode(fks_codepage_t dstCP, char dst[], size_t dstSz, Fks_MbcEnv const* srcMbc, char const* src, size_t srcSz);
//size_t   fks_mbc_unicodeFromDbc(Fks_MbcEnv const* dstMbc, char dst[], size_t dstSz, fks_codepage_t srcCP, char const* src, size_t srcSz);
#endif

Fks_MbcEnv const* fks_mbc_checkJpEncode(char const* src, size_t len, Fks_MbcEnv const* dflt FKS_ARG_INI(0));


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace fks {


template<class D, class S>
D& CharEncodeConvJp(D& dst, Fks_MbcEnv const* dstEnc, S const& src, Fks_MbcEnv const* srcEnc, bool autoFlag, bool* pErr=NULL) {
	if (autoFlag) {
		Fks_MbcEnv const* enc = fks_mbcEnv_tinyCheckJpEncode(&src[0], src.size());
		if (enc != NULL)
			srcEnc = enc;
	}
	if (srcEnc == NULL)
		srcEnc = fks_mbc_utf8;
	return dst;
}

} // fks
#endif


#endif  /* FKS_MBC_H_INCLUDED */
