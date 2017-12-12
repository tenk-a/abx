/**
 *  @file   cinttypes
 *  @brief  Format conversion of integer types
 */
#ifndef __CWRAP_INTTYPES_H
#define __CWRAP_INTTYPES_H
#pragma once

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct imaxdiv_t {
    intmax_t	quot;
    intmax_t	rem;
} imaxdiv_t;

static __inline intmax_t   imaxabs(intmax_t j) { return (j < 0) ? -j : j; }
static __inline imaxdiv_t  imaxdiv(intmax_t number, intmax_t denom) { imaxdiv_t d; d.quot = number / denom; d.rem = number % denom; return d; }

#if 0
static __inline intmax_t   strtoimax(const char * s, char** endp, int radix) { return (intmax_t)_strtoi64(s, endp, radix); }
static __inline intmax_t   strtoumax(const char * s, char** endp, int radix) { return (uintmax_t)_strtoui64(s, endp, radix); }

static __inline intmax_t   wcstoimax(const wchar_t * s, wchar_t** endp, int radix) { return (intmax_t)_wcstoi64(s, endp, radix); }
static __inline intmax_t   wcstoumax(const wchar_t * s, wchar_t** endp, int radix) { return (uintmax_t)_wcstoui64(s, endp, radix); }
#endif

#ifdef __cplusplus
}
#endif

// ==================================== ======================================= =======================================
/* macros for printf */
#if !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS)
    #define PRId8   	    "d"
    #define PRId16  	    "d"
    #define PRId32  	    "d"
    #define PRIdLEAST8	    "d"
    #define PRIdLEAST16     "d"
    #define PRIdLEAST32     "d"
    #define PRIdFAST8	    "d"
    #define PRIdFAST16	    "d"
    #define PRIdFAST32	    "d"
    #if __BORLANDC__ >= 0x560
     #define PRId64 	    "lld"
     #define PRIdLEAST64     PRId64
     #define PRIdFAST64	    PRId64
     #define PRIdMAX 	    PRId64
    #else
     //#define PRIdMAX 	    "d"
    #endif
    #if 0 //defined(_WIN64)
     #define PRIdPTR	    PRId64
    #else
     #define PRIdPTR	    "d"
    #endif

    #define PRIi8   	    "i"
    #define PRIi16  	    "i"
    #define PRIi32  	    "i"
    #define PRIiLEAST8	    "i"
    #define PRIiLEAST16     "i"
    #define PRIiLEAST32     "i"
    #define PRIiFAST8	    "i"
    #define PRIiFAST16	    "i"
    #define PRIiFAST32	    "i"
    #if __BORLANDC__ >= 0x560
     #define PRIi64 	    "lli"
     #define PRIiLEAST64    PRIi64
     #define PRIiFAST64	    PRIi64
     #define PRIiMAX 	    PRIi64
    #else
     //#define PRIiMAX 	    "i"
    #endif
    #if 0 //defined(_WIN64)
     #define PRIiPTR	    PRIi64
    #else
     #define PRIiPTR	    "i"
    #endif

    #define PRIu8   	    "u"
    #define PRIu16  	    "u"
    #define PRIu32  	    "u"
    #define PRIuLEAST8	    "u"
    #define PRIuLEAST16     "u"
    #define PRIuLEAST32     "u"
    #define PRIuFAST8	    "u"
    #define PRIuFAST16	    "u"
    #define PRIuFAST32	    "u"
    #if __BORLANDC__ >= 0x560
     #define PRIu64 	    "llu"
     #define PRIuLEAST64    PRIu64
     #define PRIuFAST64	    PRIu64
     #define PRIuMAX 	    PRIu64
    #endif
    #if 0 //defined(_WIN64)
     #define PRIuPTR	    PRIu64
    #else
     #define PRIuPTR	    "u"
    #endif

    #define PRIo8   	    "o"
    #define PRIo16  	    "o"
    #define PRIo32  	    "o"
    #define PRIoLEAST8	    "o"
    #define PRIoLEAST16     "o"
    #define PRIoLEAST32     "o"
    #define PRIoFAST8	    "o"
    #define PRIoFAST16	    "o"
    #define PRIoFAST32	    "o"
    #if __BORLANDC__ >= 0x560
     #define PRIo64 	    "llo"
     #define PRIoLEAST64    PRIo64
     #define PRIoFAST64	    PRIo64
     #define PRIoMAX 	    PRIo64
    #endif
    #if 0 //defined(_WIN64)
     #define PRIoPTR	    PRIo64
    #else
     #define PRIoPTR	    "o"
    #endif

    #define PRIx8   	    "x"
    #define PRIx16  	    "x"
    #define PRIx32  	    "x"
    #define PRIxLEAST8	    "x"
    #define PRIxLEAST16     "x"
    #define PRIxLEAST32     "x"
    #define PRIxFAST8	    "x"
    #define PRIxFAST16	    "x"
    #define PRIxFAST32	    "x"
    #if __BORLANDC__ >= 0x560
     #define PRIx64 	    "llx"
     #define PRIxLEAST64    PRIx64
     #define PRIxFAST64	    PRIx64
     #define PRIxMAX 	    PRIx64
    #endif
    #if 0 //defined(_WIN64)
     #define PRIxPTR	    PRIx64
    #else
     #define PRIxPTR	    "x"
    #endif

    #define PRIX8   	    "X"
    #define PRIX16  	    "X"
    #define PRIX32  	    "X"
    #define PRIXLEAST8	    "X"
    #define PRIXLEAST16     "X"
    #define PRIXLEAST32     "X"
    #define PRIXFAST8	    "X"
    #define PRIXFAST16	    "X"
    #define PRIXFAST32	    "X"
    #if __BORLANDC__ >= 0x560
     #define PRIX64 	    "llX"
     #define PRIXLEAST64    PRIX64
     #define PRIXFAST64	    PRIX64
     #define PRIXMAX 	    PRIX64
    #endif
    #if 0 //defined(_WIN64)
     #define PRIXPTR	    PRIX64
    #else
     #define PRIXPTR	    "X"
    #endif


    /* -------------------------------------------------------------------------*/
    /* for scanf */

    #define SCNd8   	    "hhd"
    #define SCNd16  	    "hd"
    #define SCNd32  	    "d"
    #define SCNdLEAST8	    "hhd"
    #define SCNdLEAST16     "hd"
    #define SCNdLEAST32     "d"
    #define SCNdFAST8	    "hhd"
    #define SCNdFAST16	    "hd"
    #define SCNdFAST32	    "d"
    #if __BORLANDC__ >= 0x560
     #define SCNd64 	    "lld"
     #define SCNdLEAST64    SCNd64
     #define SCNdFAST64	    SCNd64
     #define SCNdMAX 	    SCNd64
    #endif
    #if 0 //defined(_WIN64)
     #define SCNdPTR	    SCNd64
    #else
     #define SCNdPTR	    SCNd32
    #endif

    #define SCNi8   	    "hhi"
    #define SCNi16  	    "hi"
    #define SCNi32  	    "i"
    #define SCNiLEAST8	    "hhi"
    #define SCNiLEAST16     "hi"
    #define SCNiLEAST32     "i"
    #define SCNiFAST8	    "hhi"
    #define SCNiFAST16	    "hi"
    #define SCNiFAST32	    "i"
    #if __BORLANDC__ >= 0x560
     #define SCNi64 	    "lli"
     #define SCNiLEAST64    SCNi64
     #define SCNiFAST64	    SCNi64
     #define SCNiMAX 	    SCNi64
    #endif
    #if 0 //defined(_WIN64)
     #define SCNiPTR	    SCNi64
    #else
     #define SCNiPTR	    "i"
    #endif

    #define SCNu8   	    "hhu"
    #define SCNu16  	    "hu"
    #define SCNu32  	    "u"
    #define SCNuLEAST8	    "hhu"
    #define SCNuLEAST16     "hu"
    #define SCNuLEAST32     "u"
    #define SCNuFAST8	    "hhu"
    #define SCNuFAST16	    "hu"
    #define SCNuFAST32	    "u"
    #if __BORLANDC__ >= 0x560
     #define SCNu64 	    "llu"
     #define SCNuLEAST64     SCNu64
     #define SCNuFAST64	    SCNu64
     #define SCNuMAX 	    SCNu64
    #endif
    #if 0 //defined(_WIN64)
     #define SCNuPTR	    SCNu64
    #else
     #define SCNuPTR	    "u"
    #endif

    #define SCNo8   	    "hho"
    #define SCNo16  	    "ho"
    #define SCNo32  	    "o"
    #define SCNoLEAST8	    "hho"
    #define SCNoLEAST16     "ho"
    #define SCNoLEAST32     "o"
    #define SCNoFAST8	    "hho"
    #define SCNoFAST16	    "ho"
    #define SCNoFAST32	    "o"
    #if __BORLANDC__ >= 0x560
     #define SCNoLEAST64     SCNo64
     #define SCNo64 	    "llo"
     #define SCNoFAST64	    SCNo64
     #define SCNoMAX 	    SCNo64
    #endif
    #if 0 //defined(_WIN64)
     #define SCNoPTR	    SCNo64
    #else
     #define SCNoPTR	    "o"
    #endif

    #define SCNx8   	    "hhx"
    #define SCNx16  	    "hx"
    #define SCNx32  	    "x"
    #define SCNxLEAST8	    "hhx"
    #define SCNxLEAST16     "hx"
    #define SCNxLEAST32     "x"
    #define SCNxFAST8	    "hhx"
    #define SCNxFAST16	    "hx"
    #define SCNxFAST32	    "x"
    #if __BORLANDC__ >= 0x560
     #define SCNx64 	    "llx"
     #define SCNxLEAST64    SCNx64
     #define SCNxFAST64	    SCNx64
     #define SCNxMAX 	    SCNx64
    #endif
    #if 0 //defined(_WIN64)
     #define SCNxPTR	    SCNx64
    #else
     #define SCNxPTR	    "x"
    #endif
#endif
// ==================================== ======================================= =======================================


#endif
