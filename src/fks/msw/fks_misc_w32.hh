/**
 *  @file   fks_misc_w32.hh
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks/fks_common.h>

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
#ifdef _MSC_VER
#include <crtdbg.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif


// ============================================================================
FKS_LIBVA_DECL (int)
fks_abort_printf(char const* fmt, ...) FKS_NOEXCEPT
{
    {
     #if !defined(FKS_ABORT_PRINTF_TO_STDERR) && defined FKS_WIN32 && (!defined _CONSOLE || defined _DEBUG)
        va_list ap;
     #if 1
        enum { BUF_SZ = 0x1000 };     // for wsprintf
        char buf[BUF_SZ];
        va_start(ap, fmt);
        vsnprintf(buf, BUF_SZ, fmt, ap);
        buf[sizeof(buf) - 1] = '\0';
        wchar_t wbuf[BUF_SZ];
        fks_wcsFromMbs(wbuf, BUF_SZ, buf, strlen(buf)+1);
        OutputDebugStringW(wbuf);
     #else
        enum { BUF_SZ = 1024 };     // for wsprintf
        char buf[BUF_SZ];
        va_start(ap, fmt);
        wvsprintfA(buf, fmt, ap);
        buf[sizeof(buf) - 1] = '\0';
        OutputDebugStringA(buf);
     #endif
        va_end(ap);
      #if !defined(FKS_NO_ABORT)
       #if defined(_MSC_VER)
        _CrtDbgBreak();
       #else
        exit(1);
       #endif
      #endif
     #endif
    }
    {
     #if defined(_CONSOLE)
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        // ((*(char*)0) = 0);
      #if !defined(FKS_NO_ABORT)
        exit(1);
      #endif
     #endif
    }
    return 1;
}


// ============================================================================
#if 1

FKS_LIB_DECL (int)  fks_get_errno(void)
{
    return GetLastError();
}

FKS_LIB_DECL (int)  fks_set_errno(int no)
{
    SetLastError(no);
    return no;
}
#endif


// ============================================================================
#if 1
FKS_LIB_DECL (const char*)  fks_strerror(int err)
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
FKS_LIB_DECL (const wchar_t*)   fks_wcserror(int err)
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

#ifdef __cplusplus
fks_errno_cpp_t     fks_errno;
#endif

#endif
