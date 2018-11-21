/**
 *	@file	fks_misc.h
 *	@author	Masashi Kitamura (tenka@6809.net)
 *	@license Boost Software Lisence Version 1.0
 */
#ifndef FKS_MISC_H
#define FKS_MISC_H

#include <fks_config.h>

#ifdef FKS_WIN32
extern int _fks_priv_mbswcs_codepage;
#endif

FKS_LIBVA_DECL (int) fks_abort_printf(char const* fmt, ...);

#endif
