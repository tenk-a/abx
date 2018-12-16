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

typedef struct Fks_MbcEnv {
    unsigned (*isLead)(unsigned c);
    unsigned (*chkC)(unsigned c);
    unsigned (*getC)(const char** str);
    unsigned (*peekC)(const char* str);
	char*	 (*charNext)(char const* str);
    char*    (*setC)(char*  dst, unsigned c);
    unsigned (*len1)(const char* pChr);
    unsigned (*chrLen)(unsigned chr);
    unsigned (*chrWidth)(unsigned chr);
    unsigned (*chrWidthJp)(unsigned chr);
} Fks_MbcEnv;

extern Fks_MbcEnv const* const	fks_mbc_utf8;
extern Fks_MbcEnv const* const	fks_mbc_utf16le;
extern Fks_MbcEnv const* const	fks_mbc_utf16be;
extern Fks_MbcEnv const* const	fks_mbc_utf32le;
extern Fks_MbcEnv const* const	fks_mbc_utf32be;
extern Fks_MbcEnv const* const	fks_mbc_sjis;
extern Fks_MbcEnv const* const	fks_mbc_euc;
extern Fks_MbcEnv const* const	fks_mbc_eucJp;
#ifdef FKS_WIN32
extern Fks_MbcEnv const* const	fks_mbc_dbc;
#endif

size_t  fks_mbcAdjustSize(Fks_MbcEnv const* mbc, const char* str, size_t size);
size_t  fks_mbcSizeToWidth(Fks_MbcEnv const* mbc, const char* str, size_t size);
size_t  fks_mbcSizeToChrs(Fks_MbcEnv const* mbc, const char* str, size_t size);
size_t  fks_mbcChrsToWidth(Fks_MbcEnv const* mbc, const char* str, size_t chrs);
size_t  fks_mbcChrsToSize(Fks_MbcEnv const* mbc, const char* str, size_t chrs);
size_t  fks_mbcWidthToSize(Fks_MbcEnv const* mbc, const char* str, size_t width);
size_t  fks_mbcWidthToChrs(Fks_MbcEnv const* mbc, const char* str, size_t width);
char*   fks_mbcCpy(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, const char* src);
char*   fks_mbcLCpy(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, const char* src, size_t l);
char*   fks_mbcCat(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, const char* src);
char*   fks_mbcCpyNC(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, const char* src, size_t nc);
char*   fks_mbcCatNC(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, const char* src, size_t nc);
char*   fks_mbcCpyWidth(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, const char* src, size_t width);
char*   fks_mbcCatWidth(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, const char* src, size_t width);
int 	fks_mbcCmp(Fks_MbcEnv const* mbc, const char* lp, const char* rp);

#ifdef __cplusplus
}
#endif


#endif  /* FKS_MBC_H_INCLUDED */
