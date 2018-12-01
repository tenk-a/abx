/**
 *  @file   fks_io_priv_w32.h
 *  @brief
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software License Version 1.0
 */
#ifndef FKS_IO_PRIV_W32_H_INCLUDED
#define FKS_IO_PRIV_W32_H_INCLUDED

#include <fks_common.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FKS_ISALPHA(c)          (((c) <= 'z') && ((c) >= 'A') && ((c) >= 'a' || (c) <= 'Z'))

#if 1   // nano sec.
#define FKS_W32FTIME_TO_TIME(w64time)   (w64time)
#define fks_time_to_w32ftime(tm)        (tm)
#else   // micro sec.
#define FKS_W32FTIME_TO_TIME(w64time)   ((w64time) / 1000)  ///< nano sec. -> micro sec.
#define fks_time_to_w32ftime(tm)        ((tm) * 1000)       ///< micro sec. -> nano sec.
#endif

/// u32[2] -> uint64_t
#ifdef FKS_BYTE_ALIGN
#define FKS_U32X2P_TO_U64(u32x2p)       (*(uint64_t*)(u32x2p))
#elif (defined FKS_BIG_ENDIAN)
#define FKS_U32X2P_TO_U64(u32x2p)       (((uint64_t)(((uint32_t*)(u32x2p))[0]) << 32) | ((uint32_t*)(u32x2p))[1])
#else
#define FKS_U32X2P_TO_U64(u32x2p)       (((uint64_t)(((uint32_t*)(u32x2p))[1]) << 32) | ((uint32_t*)(u32x2p))[0])
#endif

/// win32-file-attributes to stat::st_mode
#define FKS_W32FATTER_TO_STATMODE(w32atr)                                                   \
        (   ( (((w32atr) & FILE_ATTRIBUTE_READONLY ) ? 0555 : 0777)                         \
             |(((w32atr) & FILE_ATTRIBUTE_DIRECTORY) ? FKS_S_IFDIR      : FKS_S_IFREG))     \
          & (  ((w32atr) & FILE_ATTRIBUTE_HIDDEN) ? ~7 : ~0 )                               \
        )

/// stat::st_mode to win32-file-attributes
#define FKS_STATMODE_TO_W32FATTR(mode)                                              \
    (  (((mode) & FKS_S_IFDIR ) ? FILE_ATTRIBUTE_DIRECTORY   : 0)                   \
     | (((mode) & FKS_S_IWRITE)== 0 ? FILE_ATTRIBUTE_READONLY : 0)                  \
     | ((((mode) & FKS_S_IFDIR)| !((mode)&FKS_S_IWRITE))?0:FILE_ATTRIBUTE_NORMAL)   \
    )


/* ======================================================================== */

extern int _fks_priv_mbswcs_codepage;
#define FKS_WCS_FROM_MBS(d,dl,s,sl) MultiByteToWideChar(_fks_priv_mbswcs_codepage,0,(s),(sl),(d),(dl))
#define FKS_MBS_FROM_WCS(d,dl,s,sl) WideCharToMultiByte(_fks_priv_mbswcs_codepage,0,(s),(sl),(d),(dl),0,0)

#define FKS_PRIV_FH2WH(fh)          (HANDLE)(fh)
#define FKS_PRIV_FH_CONV(fh)        fks_priv_fh_conv(fh)
FKS_FAST_DECL (fks_fh_t)            fks_priv_fh_conv(fks_fh_t hdl) FKS_NOEXCEPT;

enum { FKS_LONGNAME_FROM_CS_LEN = 248/*260*/ };
#if 1 //def FKS_HAS_INLINE_ALLOCA
#define FKS_INNR_FPATH_MAX          FKS_LONGNAME_FROM_CS_LEN
#define FKS_ALLOCA(l)               fks_alloca(l)
#else
#define FKS_INNR_FPATH_MAX          4096
#define FKS_ALLOCA(l)               0
#endif

#define FKS_LONGFNAME_FROM_CS_INI(n)    wchar_t __fKs_longname_buf[n][FKS_INNR_FPATH_MAX+1]

// * Note on optimization because it depends on the memory of the local scope array variable remaining.
#define FKS_LONGFNAME_FROM_CS(__fKs_n, d, s) do {                                       \
        const char* __fKs_s  = (s);                                                     \
        wchar_t*    __fKs_d  = __fKs_longname_buf[__fKs_n];                             \
        size_t      __fKs_sl, __fKs_dl;                                                 \
        __fKs_dl = fks_priv_longfname_from_cs_subr1(__fKs_s, &__fKs_sl);                \
        if (__fKs_dl >= FKS_INNR_FPATH_MAX)                                             \
            __fKs_d = (wchar_t*)FKS_ALLOCA((__fKs_dl+6)*sizeof(wchar_t));               \
        (d) = fks_priv_longfname_from_cs_subr2(__fKs_d, __fKs_dl, __fKs_s, __fKs_sl);   \
    } while (0)

FKS_LIB_DECL(size_t)    fks_priv_longfname_from_cs_subr1(char const* s, size_t* pLen) FKS_NOEXCEPT;
FKS_LIB_DECL(wchar_t*)  fks_priv_longfname_from_cs_subr2(wchar_t* d, size_t dl, char const* s, size_t sl) FKS_NOEXCEPT;


#ifdef __cplusplus
}
#endif

#endif  // FKS_IO_PRIV_W32_H_INCLUDED
