/*
 *  @file   fks_mbc.h
 *  @brief  Multi Byte Character lib.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_MBC_H_INCLUDED
#define FKS_MBC_H_INCLUDED

#include <fks_common.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FKS_WIN32
typedef int		fks_codepage_t;
enum { FKS_CP_UTF8 = 65001, FKS_CP_UTF16LE=1200, FKS_CP_UTF16BE=1201, FKS_CP_UTF32LE=12000, FKS_CP_UTF32BE=12001
	 , FKS_CP_1BYTE = 437/*kari*/, FKS_CP_SJIS = 932, FKS_CP_EUCJP = 51932/*20932*/ };
#else	// kari
typedef ptrdiff_t	fks_codepage_t;
enum { FKS_CP_UTF8 = 65001, FKS_CP_UTF16LE=1200, FKS_CP_UTF16BE=1201, FKS_CP_UTF32LE=12000, FKS_CP_UTF32BE=12001
	 , FKS_CP_1BYTE = 437/*kari*/, FKS_CP_SJIS = 932, FKS_CP_EUCJP = 51932/*20932*/ };
#endif

typedef struct Fks_MbcEnc {
	fks_codepage_t	cp;
    unsigned (*isLead)(unsigned c);
    unsigned (*chkC)(unsigned c);
    unsigned (*getC)(char const** str);
    unsigned (*peekC)(char const* str);
	char*	 (*charNext)(char const* str);
    char*    (*setC)(char* dst, char* e, unsigned c);
    unsigned (*len1)(char const* pChr);
    unsigned (*chrLen)(unsigned chr);
    unsigned (*chrWidth)(unsigned chr);
	size_t   (*adjustSize)(char const* str, size_t size);
	int 	 (*cmp)(char const* lp, char const* rp);
	int		 (*checkEncoding)(char const* str, size_t size, int lastBrokenOk);
} Fks_MbcEnc;

typedef Fks_MbcEnc const*	fks_mbcenc_t;

extern fks_mbcenc_t const	fks_mbc_utf8;
extern fks_mbcenc_t const	fks_mbc_utf16le;
extern fks_mbcenc_t const	fks_mbc_utf16be;
extern fks_mbcenc_t const	fks_mbc_utf32le;
extern fks_mbcenc_t const	fks_mbc_utf32be;
extern fks_mbcenc_t const   fks_mbc_asc;
#ifdef FKS_WIN32
extern fks_mbcenc_t const	fks_mbc_dbc;
extern fks_mbcenc_t const	fks_mbc_cp932;
#endif

#ifdef FKS_USE_MBC_JIS
extern fks_mbcenc_t const	fks_mbc_sjisX213;
extern fks_mbcenc_t const	fks_mbc_eucjp;
#ifndef FKS_WIN32
extern fks_mbcenc_t const	fks_mbc_cp932;
#endif
#endif

int  	fks_mbsCheckEncoding(fks_mbcenc_t mbc, char const* s, size_t len, int lastBrokenOk);
size_t  fks_mbsChrsToSize(fks_mbcenc_t mbc, char const* str, size_t chrs);
size_t  fks_mbsSizeToChrs(fks_mbcenc_t mbc, char const* str, size_t size);
size_t  fks_mbsSizeToWidth(fks_mbcenc_t mbc, char const* str, size_t size);
size_t  fks_mbsChrsToWidth(fks_mbcenc_t mbc, char const* str, size_t chrs);
size_t  fks_mbsWidthToSize(fks_mbcenc_t mbc, char const* str, size_t width);
size_t  fks_mbsWidthToChrs(fks_mbcenc_t mbc, char const* str, size_t width);
size_t  fks_mbsCpy(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src);
size_t  fks_mbsLCpy(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t l);
size_t  fks_mbsCat(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src);
size_t  fks_mbsCpyNC(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t nc);
size_t  fks_mbsCatNC(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t nc);
size_t  fks_mbsCpyWidth(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t width);
size_t  fks_mbsCatWidth(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t width);

size_t	fks_mbsCountCapa(fks_mbcenc_t dstMbc, fks_mbcenc_t srcMbc, char const* src, size_t srcSz);
size_t	fks_mbsConv(fks_mbcenc_t dstMbc, char dst[], size_t dstSz, fks_mbcenc_t srcMbc, char const* src, size_t srcSz);
fks_mbcenc_t fks_mbsAutoCharEncoding(char const* src, size_t len, int canEndBroken FKS_ARG_INI(0), fks_mbcenc_t *tbl FKS_ARG_INI(0), size_t tblN FKS_ARG_INI(0));

size_t  fks_mbsConvUnicode(fks_mbcenc_t dstMbc, char dst[], size_t dstSz, fks_mbcenc_t srcMbc, char const* src, size_t srcSz);
fks_mbcenc_t fks_mbsCheckUnicodeBOM(char const* src, size_t len);
int 	fks_mbsCheckUnicodeBOMi(char const* src, size_t len);	///< 0:non 1:utf8-BOM 2:utf16le-BOM 3:utf16be-BOM 4:utf32le-BOM 5:utf32be-BOM
int 	fks_mbsCheckUTF8(char const* src, size_t len, int lastBrokenOk);	///< 0:not  1:ascii(<=7f) >=2:utf8

#ifdef FKS_WIN32
fks_mbcenc_t fks_mbc_makeDBC(Fks_MbcEnc* mbcEnv, fks_codepage_t cp);
//size_t   fks_mbc_convCP(fks_codepage_t dcp, char d[], size_t dl, fks_codepage_t scp, char const* s, size_t sl);
#endif

#ifdef FKS_USE_MBC_JIS
size_t  fks_mbsConvJisType(fks_mbcenc_t dstEnc, char dst[], size_t dstSz, fks_mbcenc_t srcEnc, char const* src, size_t srcSz);
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

namespace fks {

template<class V>
fks_mbcenc_t AutoCharEncoding(V& v, int canEndBroken=0) {
	return  fks_mbsAutoCharEncoding(&v[0], v.size(), canEndBroken, NULL, 0);
}

template<class V, class T>
fks_mbcenc_t AutoCharEncoding(V& v, T& tbl, int canEndBroken=0) {
	return  fks_mbsAutoCharEncoding(&v[0], v.size(), canEndBroken, &tbl[0], tbl.size());
}

template<class V>
fks_mbcenc_t AutoCharEncoding(V& v, fks_mbcenc_t *tbl, size_t tblNum, int canEndBroken=0) {
	return  fks_mbsAutoCharEncoding(&v[0], v.size(), canEndBroken, tbl, tblNum);
}

template<class D, class S>
D& ConvCharEncoding(D& dst, fks_mbcenc_t dstEnc, S const& src, fks_mbcenc_t srcEnc) {
	dst.resize(0);
	if (srcEnc == NULL)
		srcEnc = fks_mbc_utf8;
	size_t l = fks_mbsCountCapa(dstEnc, srcEnc, &src[0], src.size());
	if (l > 0) {
		dst.resize(l+1);
		l = fks_mbsConv(dstEnc, &dst[0], l, srcEnc, &src[0], src.size());
		dst[l] = 0;
		dst.resize(l);
	}
	return dst;
}

} // fks
#endif


#endif  /* FKS_MBC_H_INCLUDED */
