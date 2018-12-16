/**
 *  @file   fks_misc.h
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_MISC_H_INCLUDED
#define FKS_MISC_H_INCLUDED

#include <fks_common.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL (char const*)  fks_skipSpc(char const* s);
FKS_LIB_DECL (char const*)  fks_skipNotSpc(char const* s);

#ifdef __cplusplus
}
#endif

#endif  // FKS_MISC_H_INCLUDED
