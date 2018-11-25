/**
 *	@file	fks_misc_w32.cpp
 *	@author	Masashi Kitamura (tenka@6809.net)
 *	@license Boost Software Lisence Version 1.0
 */

#include <fks/fks_config.h>

#ifdef FKS_WIN32

#include <fks/fks_misc.h>
#include <fks/fks_path.h>
#include <fks/fks_io.h>
#include <fks/fks_errno.h>
#include <fks/fks_assert_ex.h>
#include <fks/fks_malloc.h>
#include <fks/msw/fks_io_priv_w32.h>
#include <stdio.h>

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================

#ifdef FKS_PATH_UTF8
int _fks_priv_mbswcs_codepage = 65001;
#else
int _fks_priv_mbswcs_codepage = 0;
#endif

FKS_LIB_DECL (void)
fks_initMB(void)
{
	int cp = GetConsoleOutputCP();  // コンソール出力のコードページ
	_fks_priv_mbswcs_codepage = cp;
	fks_pathSetUtf8(cp == 65001);
}


#ifdef FKS_USE_LONGFNAME
FKS_LIB_DECL (char**)
fks_convArgWcsToMbs(int argc, wchar_t * srcArgv[])
{
	char** argv;
	FKS_ARG_PTR_ASSERT(2, srcArgv);
	argv = (char**)fks_calloc(1, sizeof(char*) * (argc + 1));
	if (!argv) {
		FKS_ASSERT(argv != NULL && "Not enough memory.");
		return NULL;
	}
	for (int i = 0; i < argc; ++i) {
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


// ============================================================================
FKS_LIBVA_DECL (int)
fks_abort_printf(char const* fmt, ...) FKS_NOEXCEPT
{
 #if defined FKS_WIN32 && !defined _CONSOLE
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

// ============================================================================
#if 1
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
#endif


// ============================================================================
#if 1
FKS_LIB_DECL (const char*)	fks_strerror(int err)
{
	char* pMsgBuf = (char*)"";
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&pMsgBuf,
		0,
		NULL
	);
	return pMsgBuf;
}
#endif

#if 1
FKS_LIB_DECL (const wchar_t*)	fks_wcserror(int err)
{
	wchar_t* pMsgBuf = (wchar_t*)L"";
	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(wchar_t*)&pMsgBuf,
		0,
		NULL
	);
	return pMsgBuf;
}
#endif

#ifdef __cplusplus
}
#endif

#endif
