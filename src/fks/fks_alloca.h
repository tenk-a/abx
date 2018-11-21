/**
 *	@file	fks_alloca.h
 *	@brief	alloca wrapper
 *  @author Masashi Kitamura (tenka@6809.net)
 *	@date	2000,2011,2016
 *	@license Boost Software License Version 1.0
 */
#ifndef FKS_ALLOCA_H
#define FKS_ALLOCA_H

#include <fks/fks_config.h>
#include <stddef.h>

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

#endif
