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
FKS_LIB_DECL (void) 		fks_ioIsJapan(void);
FKS_LIB_DECL (fks_isize_t)	fks_wcsFromMbs(wchar_t d[], size_t dl, char const* s, size_t sl);
FKS_LIB_DECL (fks_isize_t)	fks_mbsFromWcs(char d[], size_t dl, wchar_t const* s, size_t sl);
FKS_LIB_DECL (char*)		fks_ioMbsToOutput(char d[], size_t dl, char const* s);
FKS_LIB_DECL (char*)		fks_ioMbsFromOutput(char d[], size_t dl, char const* s);
FKS_LIB_DECL (int)			fks_ioMbcLenMaxI(void);
FKS_LIB_DECL (int)			fks_ioMbcLenMaxO(void);

FKS_LIB_DECL (char**)		fks_convArgWcsToMbs(int argc, wchar_t * srcArgv[]);

#ifdef FKS_WIN32
FKS_LIB_DECL (char*)		fks_w32mbsConv(int dcp, char d[], size_t dl, int scp, char const* s);
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class Fks_IoMbs2Out {
	enum {SBUF_SZ = 130};
	char	sbuf_[SBUF_SZ];
public:
	char*	p;
	Fks_IoMbs2Out(char const* msg);
	~Fks_IoMbs2Out();
};
#define FKS_MBSO(s)		Fks_IoMbs2Out(s).p
#endif

#endif	// FKS_MBC_H_INCLUDED
