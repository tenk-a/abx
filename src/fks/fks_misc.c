/**
 *  @file   fks_misc.c
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks_common.h>

#ifdef FKS_WIN32
#include "msw/fks_misc_w32.hh"
#elif 1 //defined FKS_LINUX // || defined FKS_BSD
//#include "uni/fks_misc_uni.hh"
#endif

#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL (char const*)
fks_skipSpc(char const* s)
{
    while (*s && *(unsigned char *)s <= ' ')
        s++;
    return s;
}


FKS_LIB_DECL (char const*)
fks_skipNotSpc(char const* s)
{
    while (*(unsigned char *)s > ' ')
        s++;
    return s;
}


#ifdef __cplusplus
}
#endif
