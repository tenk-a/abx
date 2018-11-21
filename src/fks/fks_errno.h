/**
 *	@file	errno.h
 *	@brief	Errors
 *	@author	tenk* (Masashi Kitamura)
 *	@date	2000-2016
 *	@license	Boost Software Lisence Version 1.0
 */
#ifndef FKS_ERRNO_H
#define FKS_ERRNO_H

#include <fks_config.h>

#if defined(FKS_USE_WIN_API)
 #include <mswin/fks_errno_w32.h>
#else
 #include <errno.h>
#endif

#endif /* FKS_ERRNO_H */
