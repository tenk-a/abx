/**
 *  @file   errno.h
 *  @brief  Errors
 *  @author tenk* (Masashi Kitamura)
 *  @date   2000-2016
 *  @license    Boost Software Lisence Version 1.0
 */
#ifndef FKS_ERRNO_H_INCLUDED
#define FKS_ERRNO_H_INCLUDED

#include <fks/fks_common.h>

#if defined(FKS_WIN32)
 #include <fks/msw/fks_errno_w32.h>
#else
 #include <errno.h>
 #define FKS_EDOM       EDOM
 #define FKS_ERANGE     ERANGE
 #define FKS_EILSEQ     EILSEQ
#endif

#endif /* FKS_ERRNO_H_INCLUDED */
