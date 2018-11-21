/**
 *	@file	fks_errno.c
 *	@brief	Errors
 *  @author Masashi Kitamura (tenka@6809.net)
 *	@date	2010-2016
 *	@license Boost Software Lisence Version 1.0
 */

#include "fks_errno.h"

#if FKS_WIN32

#ifdef __cplusplus
fks_errno_cpp_t		fks_errno;
#endif

FKS_LIB_DECL (int)	fks_get_errno(void)
{
	return FKS_W32 GetLastError();
}

FKS_LIB_DECL (int)	fks_set_errno(int no)
{
	FKS_W32 SetLastError(no);
	return no;
}

#endif
