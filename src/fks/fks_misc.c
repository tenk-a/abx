/**
 *	@file	fks_misc.h
 *	@author	Masashi Kitamura (tenka@6809.net)
 *	@license Boost Software Lisence Version 1.0
 */
#include <fks_misc.h>


int _fks_priv_mbswcs_codepage = 0;


FKS_LIBVA_DECL (int)
fks_abort_printf(char const* fmt, ...)
{
	va_list	ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	return 1;
}
