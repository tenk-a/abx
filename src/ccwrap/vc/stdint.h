/**
 *  @file   stdint.h
 *  @brief  stdint(typedef int??_t) for vc(<=15.??)
 *  @date   2000-2017
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @note
 *   -	public domain software
 */
#pragma once

#include <ccwrap_header.h>

#if (_MSC_VER >= 1600)

#include __CCWRAP_NATIVE_C_HEADER_PATH(stdint.h)

#else	// _MSC_VER

#include <stddef.h>

typedef __int8	    	    int8_t;
typedef unsigned __int8     uint8_t;
typedef __int16     	    int16_t;
typedef unsigned __int16    uint16_t;
typedef __int32     	    int32_t;
typedef unsigned __int32    uint32_t;
typedef __int64     	    int64_t;
typedef unsigned __int64    uint64_t;

#if defined(_WIN64)
 typedef int64_t    	    intptr_t;
 typedef uint64_t   	    uintptr_t;
#else
 typedef int32_t    	    intptr_t;
 typedef uint32_t   	    uintptr_t;
#endif

#if !defined(__CCWRAP_NO_INT64)
 typedef int64_t    	    intmax_t;
 typedef uint64_t   	    uintmax_t;
#else
 typedef int32_t    	    intmax_t;
 typedef uint32_t   	    uintmax_t;
#endif

typedef int8_t	    	    int_least8_t;
typedef uint8_t     	    uint_least8_t;
typedef int16_t     	    int_least16_t;
typedef uint16_t    	    uint_least16_t;
typedef int32_t     	    int_least32_t;
typedef uint32_t    	    uint_least32_t;
typedef int64_t     	    int_least64_t;
typedef uint64_t    	    uint_least64_t;

typedef int8_t	    	    int_fast8_t;
typedef uint8_t     	    uint_fast8_t;
typedef int16_t     	    int_fast16_t;
typedef uint16_t    	    uint_fast16_t;
typedef int32_t     	    int_fast32_t;
typedef uint32_t    	    uint_fast32_t;
typedef int64_t     	    int_fast64_t;
typedef uint64_t    	    uint_fast64_t;


#if !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS)

#define INT8_MIN    	    (-127-1)
#define INT8_MAX    	      127
#define UINT8_MAX   	      255

#define INT16_MIN   	    (-32767-1)
#define INT16_MAX   	      32767
#define UINT16_MAX  	      65535

#define INT32_MIN   	    (-2147483647-1)
#define INT32_MAX   	      2147483647
#define UINT32_MAX  	      0xFFFFFFFF

#define INT64_MIN   	    (-9223372036854775807LL-1LL)
#define INT64_MAX   	      9223372036854775807LL
#define UINT64_MAX  	       0xFFFFFFFFFFFFFFFFULL

#if !defined(__CCWRAP_NO_INT64)
 #define INTMAX_MIN 	    INT64_MIN
 #define INTMAX_MAX 	    INT64_MAX
 #define UINTMAX_MAX	    UINT64_MAX
#else
 #define INTMAX_MIN 	    INT32_MIN
 #define INTMAX_MAX 	    INT32_MAX
 #define UINTMAX_MAX	    UINT32_MAX
#endif

#if defined(_WIN64)
 #define INTPTR_MIN 	    INT64_MIN
 #define INTPTR_MAX 	    INT64_MAX
 #define UINTPTR_MAX	    UINT64_MAX
 #define PTRDIFF_MIN	    INT64_MIN
 #define PTRDIFF_MAX	    INT64_MAX
#else
 #define INTPTR_MIN 	    INT32_MIN
 #define INTPTR_MAX 	    INT32_MAX
 #define UINTPTR_MAX	    UINT32_MAX
 #define PTRDIFF_MIN	    INT32_MIN
 #define PTRDIFF_MAX	    INT32_MAX
#endif

#define INT_LEAST8_MIN	    INT8_MIN
#define INT_LEAST8_MAX	    INT8_MAX
#define UINT_LEAST8_MAX     UINT8_MAX

#define INT_LEAST16_MIN     INT16_MIN
#define INT_LEAST16_MAX     INT16_MAX
#define UINT_LEAST16_MAX    UINT16_MAX

#define INT_LEAST32_MIN     INT32_MIN
#define INT_LEAST32_MAX     INT32_MAX
#define UINT_LEAST32_MAX    UINT32_MAX

#define INT_LEAST64_MIN     INT64_MIN
#define INT_LEAST64_MAX     INT64_MAX
#define UINT_LEAST64_MAX    UINT64_MAX


#define INT_FAST8_MIN	    INT8_MIN
#define INT_FAST8_MAX	    INT8_MAX
#define UINT_FAST8_MAX	    UINT8_MAX

#define INT_FAST16_MIN	    INT16_MIN
#define INT_FAST16_MAX	    INT16_MAX
#define UINT_FAST16_MAX     UINT16_MAX

#define INT_FAST32_MIN	    INT32_MIN
#define INT_FAST32_MAX	    INT32_MAX
#define UINT_FAST32_MAX     UINT32_MAX

#define INT_FAST64_MIN	    INT64_MIN
#define INT_FAST64_MAX	    INT64_MAX
#define UINT_FAST64_MAX     UINT64_MAX

#ifndef SIZE_MAX
#ifdef _WIN64
#define SIZE_MAX    	    UINT64_MAX
#else
#define SIZE_MAX    	    UINT32_MAX
#endif
#endif

#define SIG_ATOMIC_MIN	    INT32_MIN
#define SIG_ATOMIC_MAX	    INT32_MAX


#if 0 //def _WCHAR_IS_UINT32
 #if !defined(WCHAR_MIN)
  #define WCHAR_MIN 	    0
 #endif
 #if !defined(WCHAR_MAX)
  #define WCHAR_MAX 	    UINT32_MAX
 #endif
 #define WINT_MIN   	    UINT32_MIN
 #define WINT_MAX   	    UINT32_MAX
#else
 #if !defined(WCHAR_MIN)
  #define WCHAR_MIN 	    0
 #endif
 #if !defined(WCHAR_MAX)
  #define WCHAR_MAX 	    0xffff
 #endif
 #define WINT_MIN   	    UINT16_MIN
 #define WINT_MAX   	    UINT16_MAX
#endif

#define INT8_C(val) 	    val
#define UINT8_C(val)	    val
#define INT16_C(val)	    val
#define UINT16_C(val)	    val
#define INT32_C(val)	    (INT32_MAX - INT32_MAX + (val))
#define UINT32_C(val)	    (UINT32_MAX - UINT32_MAX + (val))
#define INT64_C(val)	    (INT64_MAX - INT64_MAX + (val))		// val##LL
#define UINT64_C(val)	    (UINT64_MAX - UINT64_MAX + (val))	// val##ULL
#define INTMAX_C(val)	    (INTMAX_MAX - INTMAX_MAX + (val))	// val##LL
#define UINTMAX_C(val)	    (UINTMAX_MAX - UINTMAX_MAX + (val))	// val##ULL

#endif	// !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS)

#endif	// _MSC_VER
