/**
 *	@file	fks_misc.h
 *	@author	Masashi Kitamura (tenka@6809.net)
 *	@license Boost Software Lisence Version 1.0
 */
#ifndef FKS_MISC_H
#define FKS_MISC_H

#include <fks/fks_config.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifdef FKS_WIN32
extern int _fks_priv_mbswcs_codepage;
#endif

FKS_LIB_DECL (void)   fks_initMB(void);
FKS_LIB_DECL (char**) fks_convArgWcsToMbs(int argc, wchar_t* srcArgv[]);

#ifdef __cplusplus
}
#endif

#endif
