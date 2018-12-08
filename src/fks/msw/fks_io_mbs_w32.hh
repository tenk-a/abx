/**
 *  @file   fks_misc_w32.hh
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks_common.h>

#ifdef FKS_WIN32

#include <fks_io_mbs.h>
#include <fks_misc.h>
#include <fks_path.h>
#include <fks_io.h>
#include <fks_errno.h>
#include <fks_assert_ex.h>
#include <fks_malloc.h>
#include <fks_alloca.h>
#include "fks_io_priv_w32.h"

#include <stdio.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifdef FKS_PATH_UTF8
fks_codepage_t fks_io_mbs_codepage = FKS_CP_UTF8;
#else
fks_codepage_t fks_io_mbs_codepage;
#endif
fks_codepage_t fks_io_mbs_env_codepage;
fks_codepage_t fks_io_mbs_output_codepage;


FKS_LIB_DECL (void)
fks_ioMbsInit(int inUtf8flag, int outUtf8flag)
{
	int cp = GetConsoleCP();
	fks_io_mbs_env_codepage		= cp;
    fks_io_mbs_codepage        	= (inUtf8flag ) ? FKS_CP_UTF8 : cp;
    fks_io_mbs_output_codepage 	= (outUtf8flag) ? FKS_CP_UTF8 : cp;
	fks_pathSetUtf8(inUtf8flag != 0);
}

FKS_LIB_DECL (void)
fks_ioMbsOutputInit(int outUtf8flag)
{
	int cp = GetConsoleCP();
    fks_io_mbs_output_codepage = (outUtf8flag) ? FKS_CP_UTF8 : cp;
}

#if 1 //def FKS_USE_JAPAN
FKS_LIB_DECL (int)	 		fks_ioIsJapan(void)
{
	return (GetUserDefaultLCID() == 1041);
}
#endif

#if 0
FKS_LIB_DECL (int)
fks_ioMbcLenMaxI(void)
{
	if (fks_io_mbs_codepage == FKS_CP_UTF8)
		return 6;
	else
		return 2;
}

FKS_LIB_DECL (int)
fks_ioMbcLenMaxO(void)
{
	if (fks_io_mbs_output_codepage == FKS_CP_UTF8)
		return 6;
	else
		return 2;
}
#endif

FKS_LIB_DECL (fks_isize_t)
fks_wcsFromMbs(wchar_t d[], size_t dl, char const* s, size_t sl)
{
	return (fks_isize_t)MultiByteToWideChar(fks_io_mbs_codepage,0,s,sl,d,dl);
}

FKS_LIB_DECL (fks_isize_t)
fks_mbsFromWcs(char d[], size_t dl, wchar_t const* s, size_t sl)
{
	return (fks_isize_t)WideCharToMultiByte(fks_io_mbs_codepage,0,s, sl,d,dl,0,0);
}

FKS_LIB_DECL (char*)
fks_ioMbsToOutput(char d[], size_t dl, char const* s)
{
	return fks_mbsConvCP(fks_io_mbs_output_codepage, d, dl, fks_io_mbs_codepage, s);
}

FKS_LIB_DECL (char*)
fks_ioMbsFromOutput(char d[], size_t dl, char const* s)
{
	return fks_mbsConvCP(fks_io_mbs_codepage, d, dl, fks_io_mbs_output_codepage, s);
}

FKS_LIB_DECL (char*)
fks_mbsConvCP(fks_codepage_t dcp, char d[], size_t dl, fks_codepage_t scp, char const* s)
{
	size_t   sl;
	FKS_ARG_PTR_ASSERT(1, d);
	FKS_ARG_ASSERT(2, dl > 1);
	FKS_ARG_PTR_ASSERT(3, s);
	sl = strlen(s) + 1;
	if (dcp != scp) {
		size_t	 bl;
		size_t   wl = MultiByteToWideChar(scp,0,s,sl,NULL,0);
		wchar_t* w  = (wchar_t*)fks_alloca(wl*2+2);
		if (!w)
			return 0;
		MultiByteToWideChar(scp,0,s,sl,w,wl);
		bl = WideCharToMultiByte(dcp,0,w,wl,NULL,0,0,0) + 1;
		if (dl > bl)
			dl = bl;
		WideCharToMultiByte(dcp,0,w,wl,d,dl,0,0);
		return d;
	} else {
		size_t sl = strlen(s) + 1;
		if (dl >= sl) {
			dl = sl;
		} else {
			// if (dl < 1) return NULL;
			// dl = fks_mbsAdjustSize(d, dl-1);
		}
		d[dl-1] = 0;
		if (d == s)
			return d;
		return (char*)memmove(d, s, dl);
	}
}

FKS_LIB_DECL(size_t)
fks_priv_longfname_from_cs_subr1(char const* s, size_t* pLen) FKS_NOEXCEPT
{
    size_t  len = strlen(s);
    *pLen = len;
    return FKS_WCS_FROM_MBS(0,0,s,len);
}


FKS_LIB_DECL(wchar_t*)
fks_priv_longfname_from_cs_subr2(wchar_t* d, size_t dl, char const* s, size_t sl) FKS_NOEXCEPT
{
    if (d) {
	    wchar_t* d2 = d;
        //if (dl >= FKS_LONGNAME_FROM_CS_LEN && (s[0] != '\\' || s[1] != '\\' || s[2] != '?' || s[3] != '\\'))
        if (dl >= FKS_LONGNAME_FROM_CS_LEN && FKS_ISALPHA(s[0]) && (s[1] == ':') && (s[2] == '\\' || s[2] == '/'))
        {
            d[0] = d[1] = d[3] = (wchar_t)'\\';
            d[2] = (wchar_t)'?';
            d2   += 4;
        }
        FKS_WCS_FROM_MBS(d2, dl, s, sl);
        d2[dl] = 0;
    } else {
		static wchar_t dmy[1] = {0};
        FKS_ASSERT(0 && "Path name is too long.");
        d = dmy;
    }
    return d;
}


#ifdef FKS_USE_LONGFNAME
FKS_LIB_DECL (char**)
fks_convArgWcsToMbs(int argc, wchar_t * srcArgv[])
{
    char** argv;
    int    i;
    FKS_ARG_PTR_ASSERT(2, srcArgv);
    argv = (char**)fks_calloc(1, sizeof(char*) * (argc + 1));
    if (!argv) {
        FKS_ASSERT(argv != NULL && "Not enough memory.");
        return NULL;
    }
    for (i = 0; i < argc; ++i) {
        size_t wl = wcslen(srcArgv[i]);
        size_t l  = FKS_MBS_FROM_WCS(NULL,0,srcArgv[i], wl) + 1;
        char*  path = (char*)fks_malloc(l);
        if (!path)
            continue;
        FKS_MBS_FROM_WCS(path, l, srcArgv[i], wl+1);
        argv[i] = path;
    }
    return argv;
}
#endif


#ifdef __cplusplus
}
#endif


#endif
