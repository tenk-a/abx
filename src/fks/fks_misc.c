/**
 *	@file	fks_misc.c
 *	@author	Masashi Kitamura (tenka@6809.net)
 *	@license Boost Software Lisence Version 1.0
 */

#include <fks/fks_common.h>

#ifdef FKS_WIN32
#include "msw/fks_misc_w32.hh"
#elif defined FKS_LINUX // || defined FKS_BSD
#include "linux/fks_misc_linux.hh"
#endif
