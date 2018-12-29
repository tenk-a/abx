/*
 *  @file   fks_malloc.h
 *  @brief
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_MALLOC_H_INCLUDED
#define FKS_MALLOC_H_INCLUDED

#include <fks_common.h>

#if 1
#include <stdlib.h>

#define fks_malloc      malloc
#define fks_calloc      calloc
#define fks_realloc     realloc
#define fks_free        free

#else
#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL(void*)         fks_malloc(fks_size_t size);
FKS_LIB_DECL(void*)         fks_calloc(fks_size_t nmemb, fks_size_t size);
FKS_LIB_DECL(void*)         fks_realloc(void* ptr, fks_size_t size);
FKS_LIB_DECL(void)          fks_free(void* ptr);

#ifdef __cplusplus
}
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) || defined(__clang__) || defined(__PCC__)
 #define fks_alloca(n)       __builtin_alloca((n))
#elif defined(__BORLANDC__)
 FKS_C_DECL(void*)           __alloca__ (size_t n);
 #define fks_alloca(n)       __alloca__(n)
#elif defined(_MSC_VER) || defined(__TINYC__) || defined(__POCC__) || defined(__LCC__)
 FKS_C_DECL(void*)           _alloca(size_t n) FKS_NOEXCEPT;
 #define fks_alloca(n)       _alloca(n)
 #ifdef _MSC_VER
  #pragma intrinsic ( _alloca )
 #endif
#else
 #include <alloca.h>
 //FKS_C_DECL(void*)         alloca(size_t n) FKS_NOEXCEPT;
 #define fks_alloca(n)       alloca(n)
#endif

#ifdef __cplusplus
}
#endif


#endif  // FKS_MALLOC_H_INCLUDED
