/*
 *  @file   fks_inOutMbs.c
 *  @brief  multi byte character routine
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_MBC_H_INCLUDED
#define FKS_MBC_H_INCLUDED

#include <fks_common.h>
#include <fks_types.h>
//#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL (void) 		fks_ioMbsInit(int inUtf8flag FKS_ARG_INI(0), int outUtf8flag FKS_ARG_INI(0));
FKS_LIB_DECL (void) 		fks_ioMbsOutputInit(int outUtf8flag);
FKS_LIB_DECL (fks_isize_t)	fks_wcsFromMbs(wchar_t d[], size_t dl, char const* s, size_t sl);
FKS_LIB_DECL (fks_isize_t)	fks_mbsFromWcs(char d[], size_t dl, wchar_t const* s, size_t sl);
FKS_LIB_DECL (char*)		fks_ioMbsToOutput(char d[], size_t dl, char const* s);
FKS_LIB_DECL (char*)		fks_ioMbsFromOutput(char d[], size_t dl, char const* s);
//FKS_LIB_DECL (int)		fks_ioMbcLenMaxI(void);
//FKS_LIB_DECL (int)		fks_ioMbcLenMaxO(void);

FKS_LIB_DECL (char**)		fks_convArgWcsToMbs(int argc, wchar_t * srcArgv[]);

#if 1 //def FKS_USE_JAPAN
FKS_LIB_DECL (int)	 		fks_ioIsJapan(void);
#endif

#ifdef FKS_WIN32
typedef int	fks_codepage_t;
#define FKS_CP_UTF8			65001
#define FKS_CP_SJIS			932
#else
typedef char const* fks_codepage_t;
#endif
FKS_LIB_DECL (char*)		fks_mbsConvCP(fks_codepage_t dcp, char d[], size_t dl, fks_codepage_t scp, char const* s);

#ifdef __cplusplus
}
#endif

#ifdef FKS_WIN32
#ifdef __cplusplus
class Fks_IoCPConvStr {
	enum {SBUF_SZ = 130};
	char	sbuf_[SBUF_SZ];
public:
	char*	str;
	Fks_IoCPConvStr(char const* msg, fks_codepage_t icp, fks_codepage_t ocp);
	~Fks_IoCPConvStr();
};
struct Fks_IoSrccodeToOutStr : public Fks_IoCPConvStr {
	Fks_IoSrccodeToOutStr(char const* msg);
};
struct Fks_IoMbsToOutStr : public Fks_IoCPConvStr {
	Fks_IoMbsToOutStr(char const* msg, bool outUtf8Flag=false);
};

#define FKS_OUT_S(s,utf8)	Fks_IoMbsToOutStr((s),(utf8)).str
#define FKS_MBS_S(s)		Fks_IoMbsToOutStr(s).str
#define FKS_UTF8_S(s)		Fks_IoMbsToOutStr((s),true).str
#define FKS_SRCCODE_S(s)	Fks_IoSrccodeToOutStr(s).str
#endif
#else
#define FKS_OUT_S(s,utf8)	(s)
#define FKS_MBS_S(s)		(s)
#define FKS_UTF8_S(s)		(s)
#define FKS_SRCCODE_S(s)	(s)
#endif

#endif	// FKS_MBC_H_INCLUDED
