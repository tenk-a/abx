/**
 *  @file   fks_misc_uni.hh
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks/fks_common.h>

#include <fks/fks_io_mbs.h>
#include <fks/fks_misc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL (void)
fks_ioMbsInit(int inUtf8flag, int outUtf8flag)
{
}

FKS_LIB_DECL (void)
fks_ioMbsOutputInit(int outUtf8flag)
{
}

static int fks_ioIsJapanFlag = -1;

FKS_LIB_DECL (int)
fks_ioIsJapan(void)
{
    if (fks_ioIsJapanFlag < 0) {
        char const* s = getenv("LANG");
        if (s && *s) {
            fks_ioIsJapanFlag = strncmp(s, "ja_JP.", 6) == 0;
        }
    }
    return fks_ioIsJapanFlag;
}

#ifdef __cplusplus
}
#endif
