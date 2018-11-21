/**
 *	@file	fks_misc.cpp
 *	@author	Masashi Kitamura (tenka@6809.net)
 *	@license Boost Software Lisence Version 1.0
 */

#include <fks_config.h>
#if FKS_WIN32
#include <fks_misc.h>
#include <stdio.h>

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

int _fks_priv_mbswcs_codepage = 0;

FKS_LIBVA_DECL (int)
fks_abort_printf(char const* fmt, ...)
{
 #if defined FKS_WIN32 && !defined FKS_CONSOLE
	enum { BUF_SZ = 1024 };		// for wsprintf
	char		buf[BUF_SZ];
	va_list	ap;
	va_start(ap, fmt);
	wvsprintfA(buf, fmt, ap);
	buf[sizeof(buf)-1] = '\0';
	OutputDebugStringA( buf );
	va_end(ap);
	return 1;
 #else
	va_list	ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	return 1;
 #endif
}


#ifdef __cplusplus
fks_errno_cpp_t		fks_errno;
#endif

FKS_LIB_DECL (int)	fks_get_errno(void)
{
	return GetLastError();
}

FKS_LIB_DECL (int)	fks_set_errno(int no)
{
	SetLastError(no);
	return no;
}


#ifdef __cplusplus
}
#endif

#endif
