/**
 *  @file   fks_common.h
 *  @brief  fks-library's common.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_COMMON_H_INCLUDED
#define FKS_COMMON_H_INCLUDED

#if 0
#ifndef __has_include
#define __has_include(x)    0
#endif
#endif

#ifdef FKS_CXX17
#if defined __has_include && __has_include("ccwrap_header.h")
#include "ccwrap_header.h"
#endif
#if defined __has_include && __has_include("fks_custom.h")
#include "fks_custom.h"
#endif
#endif

// ==================================== ======================================= =======================================
// OS
//  ex) _WIN32,_WIN64, WINVER, _WINDOWS_, _WIN32_WINNT, _MAC
//      linux,__linux,__linux__

#if defined(_WIN32) && !defined(FKS_WIN32)
 #ifndef _WIN32_WINNT
  //#define _WIN32_WINNT    0x0500// Win2k or later.
  #define _WIN32_WINNT      0x0600 // Vista or later.
 #endif
  #define FKS_WIN32         _WIN32_WINNT
#endif
#ifdef FKS_WIN32
#define FKS_USE_WIN_API
#endif

#ifndef FKS_LINUX
 #if defined linux || defined __linux || defined __linux__ || defined __LINUX__
  #define FKS_LINUX
 #endif
#endif

#ifndef FKS_OSX
 #if defined __APPLE__ || defined __APPLE_CC__
  #define FKS_APPLE
 #endif
#endif



// ==================================== ======================================= =======================================
// Compiler.
//  ex) __GNUC__,_MSC_VER, __BORLANDC__, __WATCOMC__,__INTEL_COMPILER,
//      __MWERKS__{Codewarrior}, __DMC__,__SC__{DigitalMars},

#if defined __clang__
    #define FKS_COMPILER                    "LLVM Clang Compiler"
    #if defined _WIN64 || defined __MINGW64__  || defined _M_AMD64 || defined __amd64__ || defined __AMD64__ || defined __x86_64__
     #define FKS_LDOUBLE_BIT                128
    #else
     #define FKS_LDOUBLE_BIT                96
    #endif
    #define FKS_HAS_INCLUDE_NEXT
    #if __cplusplus < 201101L
     #define FKS_NORETURN                   __attribute__((noreturn))
    #endif
    #ifdef FKS_WIN32
     #define FKS_CDECL                       __cdecl
     #define FKS_STDCALL                     __stdcall
     #define FKS_FASTCALL                    __fastcall
    #endif
    #define FKS_FORCE_INLINE                __inline__ __attribute__((always_inline))
    #define FKS_NOINLINE                    __attribute__((noinline))
    #define FKS_SELECTANY                   __attribute__((weak))
    #define FKS_DLLIMPORT                   __attribute__((dllimport))
    #define FKS_DLLEXPORT                   __attribute__((dllexport))
    #undef _MSC_VER
    #undef __GUNUC__
    #undef __MINGW__

#elif defined __DMC__
    #if __DMC__ < 0x0852
     #error "Need Digitalmars C/C++ v8.52 or later."
    #endif
    #define FKS_COMPILER                    __DMC_VERSION_STRING__
    #define FKS_LDOUBLE_BIT                 80
    #define FKS_CDECL                       __cdecl
    #define FKS_STDCALL                     __stdcall
    #define FKS_FASTCALL                    __fastcall
    #define FKS_DLLIMPORT                   __declspec(dllimport)
    #define FKS_DLLEXPORT                   __declspec(dllexport)
    #define FKS_RESTRICT                    __restrict

#elif defined __BORLANDC__
    #if __BORLANDC__ < 0x551
     #error "Need Borland C/C++ ver.5.5.1 or later."
    #endif
    #define FKS_COMPILER                    "Borland C/C++ (" FKS_M_STR(__BORLANDC__) ")"
    #define FKS_LDOUBLE_BIT                 80
    #define FKS_CDECL                       __cdecl
    #define FKS_STDCALL                     __stdcall
    #define FKS_FASTCALL                    __fastcall
    #define FKS_DLLIMPORT                   __declspec(dllimport)
    #define FKS_DLLEXPORT                   __declspec(dllexport)
    #if __BORLANDC__ < 0x561
     #define FKS_NO_VARIADIC_MACROS
     typedef __int64                        FKS_LLONG;
     typedef unsigned __int64               FKS_ULLONG;
     #define FKS_LLONG                      FKS_LLONG
     #define FKS_ULLONG                     FKS_ULLONG
     #define FKS_LONG_C(val)                val##L
     #define FKS_ULONG_C(val)               val##UL
     #define FKS_LLONG_C(val)               ((FKS_LLONG)(val))
     #define FKS_ULLONG_C(val)              ((FKS_ULLONG)(val))
     #define FKS_M_FUNC                     ""
     #if (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L)
      #define __func__                      ""
     #endif
     #define FKS_PRIF_NO_LL
     #define FKS_PRIF_LL
    #endif

#elif defined __WATCOMC__
    #if __WATCOMC__ < 1280
     #warning "Need Open Watcom C/C++ ver.1.8(__WATCOMC__=1280) or later."
    #endif
    #define FKS_COMPILER                    "Open Watcom C/C++ (" FKS_M_STR(__WATCOMC__) ")"
    #define FKS_LDOUBLE_BIT                 64
    #define FKS_CDECL                       __cdecl
    #define FKS_STDCALL                     __stdcall
    #define FKS_FASTCALL                    __fastcall
    #define FKS_DLLIMPORT                   __declspec(dllimport)
    #define FKS_DLLEXPORT                   __declspec(dllexport)

#elif defined __PCC__
    #define FKS_COMPILER                    "Portable C Compiler"
    #define FKS_LDOUBLE_BIT                 96
    #define FKS_CPU_X86

#elif defined __TINYC__
    #define FKS_COMPILER                    "Tiny C Compiler"
    #define FKS_LDOUBLE_BIT                 64
    #define FKS_WIN32                       0x0500
    #define _M_IX86

#elif defined __POCC__
    #define FKS_COMPILER                    "Pelles C Compiler"
    #define FKS_LDOUBLE_BIT                 64
    #define FKS_WIN32                       0x0500
    #define FKS_THREAD_LOCAL
    #define FKS_NO_WCHAR_T
    #define FKS_CPU_X86

#elif defined __LCC__   // lcc-win
    #define FKS_COMPILER                    "LCC-Win C Compiler"
    #define FKS_LDOUBLE_BIT                 96
    #define FKS_CPU_X86

#elif defined __ORANGEC__ || defined __CC386__
    #ifdef __ORANGEC__
     #define FKS_COMPILER                   "Orange C"
    #else
     #define FKS_COMPILER                   "cc386"
    #endif
    #define FKS_LDOUBLE_BIT                 80
    #if __STDC_VERSION__ < 199901L  // less c99
     #define FKS_M_FUNC                     ""
     typedef __int64                        FKS_LLONG;
     typedef unsigned __int64               FKS_ULLONG;
     #define FKS_LLONG                      FKS_LLONG
     #define FKS_ULLONG                     FKS_ULLONG
     #define FKS_INLINE                     __inline
     #define FKS_LLONG_C(val)               ((FKS_LLONG)(val))
     #define FKS_ULLONG_C(val)              ((FKS_ULLONG)(val))
    #endif
    #define FKS_CDECL                       __cdecl
    #define FKS_STDCALL                     __stdcall
    #define FKS_FASTCALL                    FKS_STDCALL
    #define FKS_DLLIMPORT                   _import
    #define FKS_DLLEXPORT                   _export
    #define FKS_CPU_X86
    #ifndef FKS_WIN32
     #define FKS_WIN32                      0x500
    #endif

#elif defined __GNUC__
    #define FKS_COMPILER    "GNU C/C++ Compiler (" FKS_M_STR(__GNUC__) "." FKS_M_STR(__GNUC_MINOR__) ")"
    #if defined _WIN64 || defined __MINGW64__  || defined _M_AMD64 || defined __amd64__ || defined __AMD64__ || defined __x86_64__
     #define FKS_LDOUBLE_BIT                128
    #else
     #define FKS_LDOUBLE_BIT                96
    #endif
    #define FKS_HAS_INCLUDE_NEXT
    #define FKS_RESTRICT                    __restrict
    #if __cplusplus < 201101L
     #define FKS_NORETURN                   __attribute__((noreturn))
    #endif
    #define FKS_FORCE_INLINE                __inline__ __attribute__((always_inline))
    #define FKS_NOINLINE                    __attribute__((noinline))
    #define FKS_SELECTANY                   __attribute__((weak))
    #ifdef __MINGW32__
     #define FKS_CDECL                      __cdecl
     #define FKS_STDCALL                    __stdcall
     #define FKS_FASTCALL                   __fastcall
     #define FKS_DLLIMPORT                  __attribute__((dllimport))
     #define FKS_DLLEXPORT                  __attribute__((dllexport))
     #include <_mingw.h>
    #else
     #define FKS_CDECL
     #define FKS_STDCALL
     #define FKS_FASTCALL
     #define FKS_DLLIMPORT                  __attribute__((dllimport))
     #define FKS_DLLEXPORT                  __attribute__((dllexport))
    #endif

#elif defined _MSC_VER
    #define FKS_COMPILER                    "MS Visual C/C++ (" FKS_M_STR(_MSC_VER) ")"
    #define FKS_LDOUBLE_BIT                 64
    #ifdef _CHAR_UNSIGNED
     #define FKS_IS_CHAR_UNSIGNED           1
    #else
     #define FKS_IS_CHAR_UNSIGNED           0
    #endif
    #if _MSC_VER < 1400                     // less vs2005
     #define FKS_NO_VARIADIC_MACROS
    #endif
    #define FKS_NORETURN                    __declspec(noreturn)
    #if _MSC_VER >= 1400
     #define FKS_RESTRICT                   __restrict
    #endif
    #define FKS_CDECL                       __cdecl
    #define FKS_STDCALL                     __stdcall
    #define FKS_FASTCALL                    __fastcall
    #define FKS_FORCE_INLINE                __forceinline
    #define FKS_NOINLINE                    __declspec(noinline)
    #define FKS_SELECTANY                   __declspec(selectany)
    #define FKS_DLLIMPORT                   __declspec(dllimport)
    #define FKS_DLLEXPORT                   __declspec(dllexport)

    #ifndef FKS_M_CAT
    #define FKS_M_CAT(a,b)                  FKS_M_CAT_S2(a,b)
    #define FKS_M_CAT_S2(a,b)               FKS_M_CAT_S3(a##b)
    #define FKS_M_CAT_S3(x)                 x
    #endif

    #define FKS_M_FUNC                      __FUNCTION__
    #if _MSC_VER >= 1700
     #define FKS_HAS_STATIC_ASSERT
    #endif
    #define FKS_HAS_WMAIN
    #if _MSC_VER < 1500
     #define FKS_PRIF_LL    "I64"
    #endif
#else   // unkown
 #error unkown compiler!
#endif

#if __cplusplus < 201101L
#if !defined FKS_OLD_CXX && (!defined _MSC_VER || _MSC_VER < 1800)
#define FKS_OLD_CXX
#endif
#endif

// ==================================== ======================================= =======================================
// macro

#ifndef FKS_M_CAT
#define FKS_M_CAT(a,b)                  FKS_M_CAT_S2(a,b)
#define FKS_M_CAT_S2(a,b)               a##b
#endif

#ifndef FKS_M_STR
#define FKS_M_STR(s)                    FKS_M_STR_S2(s)
#define FKS_M_STR_S2(s)                 #s
#endif

#ifndef FKS_M_FUNC
#define FKS_M_FUNC                      __func__
#endif

#ifndef FKS_PRIF_LL
 #define FKS_PRIF_LL                    "ll"
#endif

// ==================================== ======================================= =======================================
// CPU.
//  ex) (vc,bcc,mingw): _X86_, _MIPS_, _ALPHA_, _PPC_, _68K_, _MPPC_, _IA64_
//      (mwerks):    __INTEL__, _MC68K_, _MIPS_, _MIPS_ISA2~4_, _POWERPC_


// ------------------------------------
// endian & alignment
#if defined _M_IX86 || defined _X86_ || defined FKS_CPU_X86 || defined _M_AMD64 || defined __amd64__ || defined __AMD64__ || defined __x86_64__
  #define FKS_ENABLE_BYTE_ALIGN         1   // enable byte align access
  #undef  FKS_BIG_ENDIAN
  #define FKS_ENDIAN                    0   // 0:little endian  1:big endian
  #ifndef FKS_CPU_X86
   #define FKS_CPU_X86
  #endif
  #define FKS_HAS_X87
#elif defined _M_ARM || defined _ARM_ || defined __arm__ || defined FKS_CPU_ARM
  #undef  FKS_BIG_ENDIAN
  #define FKS_ENDIAN                    0
  #ifndef FKS_CPU_ARM
   #define FKS_CPU_ARM
  #endif
#elif defined _PPC_ || defined _POWERPC_ ||  || defined FKS_CPU_PPC
  #define FKS_ENABLE_BYTE_ALIGN         1   // enable byte align access
  #define FKS_BIG_ENDIAN                1
  #define FKS_ENDIAN                    1
  #ifndef FKS_CPU_PPC
   #define FKS_CPU_PPC
  #endif
#elif defined _MIPS_ || defined __mips || defined MIPS || defined FKS_CPU_MIPS
  #undef  FKS_BIG_ENDIAN
  #define FKS_ENDIAN                    0
  #ifndef FKS_CPU_MIPS
   #define FKS_CPU_MIPS
  #endif
#elif defined _68K_ || defined _MC68K_ || defined FKS_CPU_68K
  #define FKS_BIG_ENDIAN                1
  #define FKS_ENDIAN                    1
  #ifndef FKS_CPU_68K
   #define FKS_CPU_68K
  #endif
#elif defined BIG_ENDIAN
  #define FKS_BIG_ENDIAN                1
  #define FKS_ENDIAN                    1
#else
  #define FKS_ENDIAN                    0
#endif

#define FKS_ENDIAN_SEL(l,b)             ((FKS_ENDIAN)?(b):(l))



// ==================================== ======================================= =======================================

#if !defined(FKS_CPU_BIT)
 #if (defined _WIN64) ||(defined __WORDSIZE && __WORDSIZE == 64) || defined _M_AMD64 || defined _M_AMD64 || defined __amd64__ || defined __AMD64__ || defined __x86_64__ || defined _IA64_
  #define FKS_CPU_BIT   64
 #else
  #define FKS_CPU_BIT   32
 #endif
#endif

#if defined(_WIN64) || defined(FKS_WIN64) //|| defined (FKS_LINUX)
 #if !defined FKS_LP64 && !defined FKS_LLP64
  #define FKS_LLP64
 #endif
#elif FKS_CPU_BIT == 64
 #if !defined FKS_LP64 && !defined FKS_LLP64
  #define FKS_LP64
 #endif
#endif


// ==================================== ======================================= =======================================
// type bits macro

#ifndef FKS_WCHAR_BIT
 #if defined(FKS_WIN32)
  #define FKS_WCHAR_BIT                 16
 #else
  #define FKS_WCHAR_BIT                 32
 #endif
#endif
#ifndef FKS_INT_BIT
 #define FKS_INT_BIT                    32
#endif
#ifndef FKS_LONG_BIT
 #if defined(FKS_LP64)
  #define FKS_LONG_BIT                  64
 #else
  #define FKS_LONG_BIT                  32
 #endif
#endif
#ifndef FKS_LLONG_BIT
 #define FKS_LLONG_BIT                  64
#endif
#ifndef  FKS_PTR_BIT
 #if FKS_CPU_BIT == 64
  #define FKS_PTR_BIT                   64
 #else
  #define FKS_PTR_BIT                   32
 #endif
#endif
#ifndef FKS_DOUBLE_BIT
 #define FKS_DOUBLE_BIT                 64
#endif


// ==================================== ======================================= =======================================
// type define & macro

#ifndef  FKS_LONG_C
 #define FKS_LONG_C(val)                val##L
 #define FKS_ULONG_C(val)               val##UL
#endif
#ifndef  FKS_LLONG_C
 #define FKS_LLONG_C(val)               val##LL
 #define FKS_ULLONG_C(val)              val##ULL
#endif

#ifndef  FKS_BOOL
 #ifdef __cplusplus
  #define FKS_BOOL                      bool
 #elif defined(_Bool) || (!defined(FKS_NO_C_BOOL) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
  #define FKS_BOOL                      _Bool
 #else
  #define FKS_BOOL                      char
 #endif
#endif

#ifndef FKS_LLONG
 typedef long long                      FKS_LLONG;
 typedef unsigned long long             FKS_ULLONG;
 #define FKS_LLONG                      FKS_LLONG
 #define FKS_ULLONG                     FKS_ULLONG
#endif

#if FKS_INT_BIT < 32
 #define FKS_INT32_C(v)                 (FKS_LONG_C(v))
 #define FKS_UINT32_C(v)                (FKS_ULONG_C(v))
#else
 #define FKS_INT32_C(v)                 (v)
 #define FKS_UINT32_C(v)                (v)
#endif
#if   FKS_LONG_BIT == 64
 #define FKS_INT64_C(v)                 FKS_LONG_C(v)
 #define FKS_UINT64_C(v)                FKS_ULONG_C(v)
#elif FKS_LLONG_BIT == 64
 #define FKS_INT64_C(v)                 FKS_LLONG_C(v)
 #define FKS_UINT64_C(v)                FKS_ULLONG_C(v)
#endif



// ==================================== ======================================= =======================================
// decl. macro

#ifndef FKS_FORCE_INLINE
 #define FKS_FORCE_INLINE               inline
#endif
#ifndef FKS_NOINLINE
 #define FKS_NOINLINE
#endif

#if defined(__cplusplus)
 #if __cplusplus >= 201101L
  #ifndef  FKS_NORETURN
   #define FKS_NORETURN                 [[noreturn]]
  #endif
  #ifndef FKS_NOEXCEPT
   #define FKS_NOEXCEPT                 noexcept
  #endif
 #else
  #ifndef  FKS_NORETURN
   #define FKS_NORETURN
  #endif
  #ifndef FKS_NOEXCEPT
   #define FKS_NOEXCEPT                 throw()
  #endif
 #endif
 #if __cplusplus >= 201703L
  #ifndef FKS_FALLTHROUGH
   #define FKS_FALLTHROUGH              [[fallthrough]]
  #endif
 #else
  #ifndef FKS_FALLTHROUGH
   #define FKS_FALLTHROUGH
  #endif
 #endif
#else
 #if __STDC_VERSION__ >= 201112L
  #ifndef  FKS_NORETURN
   #define FKS_NORETURN                  _Noreturn
  #endif
 #else
  #ifndef  FKS_NORETURN
    #define FKS_NORETURN
  #endif
 #endif
 #ifndef FKS_NOEXCEPT
  #define FKS_NOEXCEPT
 #endif
 #ifndef  FKS_FALLTHROUGH
  #define FKS_FALLTHROUGH
 #endif
#endif

#ifdef __cplusplus
 #define FKS_EXTERN_C                   extern "C"
#else
 #define FKS_EXTERN_C                   extern
#endif

#ifndef FKS_RESTRICT
 #if __STDC_VERSION__ < 199901L || defined __cplusplus
  #define FKS_RESTRICT
 #else
  #define FKS_RESTRICT                  restrict
 #endif
#endif

#ifndef  FKS_CDECL                      // c abi
 #define FKS_CDECL
#endif
#ifndef  FKS_FASTCALL                   // fast call abi
 #define FKS_FASTCALL
#endif
#ifndef  FKS_STDCALL                    // os-abi
 #define FKS_STDCALL
#endif

#ifndef  FKS_LIBCALL                    // for this library (public)
 #define FKS_LIBCALL                    FKS_CDECL   //FKS_FASTCALL
#endif

#ifndef  FKS_DLLIMPORT
 #define FKS_DLLIMPORT
#endif
#ifndef  FKS_DLLEXPORT
 #define FKS_DLLEXPORT
#endif


// ==================================== ======================================= =======================================

#ifndef  FKS_C_DECL                     // c-abi
 #define FKS_C_DECL(t)                  t FKS_CDECL
#endif
#ifndef  FKS_FAST_DECL                  // fast call abi
 #define FKS_FAST_DECL(t)               t FKS_FASTCALL
#endif
#ifndef  FKS_STD_DECL                   // os-standerd abi
 #define FKS_STD_DECL(t)                t FKS_STDCALL
#endif

#ifndef  FKS_LIB_DECL                   // for this library declaration
 #define FKS_LIB_DECL(t)                t FKS_LIBCALL
#endif
#ifndef  FKS_LIBVA_DECL                 // for this library's variable argument function declaration
 #define FKS_LIBVA_DECL(t)              FKS_C_DECL(t)
#endif

#ifndef  FKS_FORCE_INL_LIB_DECL         // for this library's inline function
 #define FKS_FORCE_INL_LIB_DECL(t)      static FKS_FORCE_INLINE FKS_FAST_DECL(t)
#endif
#ifndef  FKS_INL_LIB_DECL               // for this library's inline function
 #define FKS_INL_LIB_DECL(t)            static inline FKS_FAST_DECL(t)
#endif
#ifndef  FKS_STATIC_DECL                // for static scope function
 #define FKS_STATIC_DECL(t)             static FKS_FAST_DECL(t)
#endif
#ifndef  FKS_INLINE_DECL                // for inline function (static scope)
 #define FKS_INLINE_DECL(t)             static inline FKS_FAST_DECL(t)
#endif
#ifndef  FKS_FORCE_INLINE_DECL         // for force inline function (static scope)
 #define FKS_FORCE_INLINE_DECL(t)      static FKS_FORCE_INLINE FKS_FAST_DECL(t)
#endif
#ifndef  FKS_OSAPI_DECL
 #define FKS_OSAPI_DECL(t)              FKS_EXTERN_C FKS_DLLIMPORT t FKS_STDCALL
#endif

#ifndef  FKS_C_PTR_DECL                 // declaration of pointer to c-abi
 #define FKS_C_PTR_DECL(t,nm)           t (FKS_CDECL* nm)
#endif
#ifndef  FKS_FAST_PTR_DECL              // declaration of pointer to fastcall-abi
 #define FKS_FAST_PTR_DECL(t,nm)        t (FKS_FASTCALL* nm)
#endif
#ifndef  FKS_STD_PTR_DECL               // declaration of pointer to os-standerd-abi
 #define FKS_STD_PTR_DECL(t,nm)         t (FKS_STDCALL* nm)
#endif
#ifndef  FKS_LIB_PTR_DECL               // declaration of pointer to this-library(function)
 #define FKS_LIB_PTR_DECL(t,nm)         t (FKS_LIBCALL* nm)
#endif

#ifdef __cplusplus
#define FKS_ARG_INI(v)                  = (v)
#else
#define FKS_ARG_INI(v)
#endif

// ==================================== ======================================= =======================================
// assertion
#ifndef FKS_STATIC_ASSERT
 #if defined(__cplusplus)
  #if __cplusplus >= 201103L || defined FKS_HAS_STATIC_ASSERT
   #define FKS_STATIC_ASSERT(c)         static_assert((c), #c " is failed.")
   #define FKS_STATIC_ASSERTMSG         static_assert
  #else
   #define FKS_STATIC_ASSERT(c)         typedef char FKS_M_CAT(Assert_L,__LINE__)[(c) ? 1/*OK*/ : -1/*NG*/]
   #define FKS_STATIC_ASSERTMSG(c,m)    FKS_STATIC_ASSERT(c)
  #endif
 #else
  #if !defined(FKS_NO_STATIC_ASSERT) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
   #define FKS_STATIC_ASSERT(c)         _Static_assert((c), #c " is failed.")
   #define FKS_STATIC_ASSERTMSG         _Static_assert
  #else
   #define FKS_STATIC_ASSERT(c)         typedef char FKS_M_CAT(Assert_L,__LINE__)[(c) ? 1/*OK*/ : -1/*NG*/]
   #define FKS_STATIC_ASSERTMSG(c,m)    FKS_STATIC_ASSERT(c)
  #endif
 #endif
#endif


#ifndef FKS_IS_RAM_PTR
#if defined _WIN64
 #define FKS_IS_RAM_PTR(p)     ((char const*)(p) >= (char const*)0x10000 && (char const*)(p) <= (char const*)0xFFFF000000000000LL)
#elif defined _WIN32
 #define FKS_IS_RAM_PTR(p)     ((char const*)(p) >= (char const*)0x10000 && (char const*)(p) <= (char const*)0xF0000000)
#elif FKS_CPU_BIT == 64
 #define FKS_IS_RAM_PTR(p)     ((char const*)(p) >= (char const*)0x100  && (char const*)(p) <= (char const*)0xFFFFffffFFFFff00)
#else
 #define FKS_IS_RAM_PTR(p)     ((char const*)(p) >= (char const*)0x100  && (char const*)(p) <= (char const*)0xFFFFff00)
#endif
#define FKS_IS_RAM_PTR0(p)      (!(p) || FKS_IS_RAM_PTR(p))
#endif


#ifndef NDEBUG
 FKS_STATIC_ASSERT(FKS_INT_BIT        == 8*sizeof(int)        );
 #ifdef __cplusplus
  FKS_STATIC_ASSERT(FKS_WCHAR_BIT     == 8*sizeof(wchar_t)    );
 #endif
 FKS_STATIC_ASSERT(FKS_LONG_BIT       == 8*sizeof(long)       );
 FKS_STATIC_ASSERT(FKS_LLONG_BIT      == 8*sizeof(FKS_LLONG)  );
 FKS_STATIC_ASSERT(FKS_DOUBLE_BIT     == 8*sizeof(double)     );
 FKS_STATIC_ASSERT(FKS_LDOUBLE_BIT    == 8*sizeof(long double));
 FKS_STATIC_ASSERT(FKS_PTR_BIT        == 8*sizeof(void*)      );
 FKS_STATIC_ASSERT(FKS_CPU_BIT == 64 || FKS_CPU_BIT == 32);
#endif

#ifdef __cplusplus
namespace fks {}

#if __cplusplus >= 201402L
 #ifndef FKS_CXX14
  #define FKS_CXX14
 #endif
#endif
#if __cplusplus >= 201703L
 #ifndef FKS_CXX17
  #define FKS_CXX17
 #endif
#endif
#ifdef FKS_CXX14
#define FKS_CONSTEXPR14 constexpr
#else
#define FKS_CONSTEXPR14
#endif

#endif


// ==================================== ======================================= =======================================

#endif  // FKS_COMMON_H_INCLUDED
