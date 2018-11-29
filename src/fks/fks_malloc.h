/*
 *  @file   fks_malloc.h
 *  @brief
 *	@author	Masashi Kitamura (tenka@6809.net)
 *	@license Boost Software Lisence Version 1.0
 */
#ifndef FKS_MALLOC_H_INCLUDED
#define FKS_MALLOC_H_INCLUDED

#include <fks/fks_common.h>

#if 1
#include <stdlib.h>

#define fks_malloc		malloc
#define fks_calloc		calloc
#define fks_realloc		realloc
#define fks_free		free

#else
#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL(void*)			fks_malloc(fks_size_t size);
FKS_LIB_DECL(void*)			fks_calloc(fks_size_t nmemb, fks_size_t size);
FKS_LIB_DECL(void*)			fks_realloc(void* ptr, fks_size_t size);
FKS_LIB_DECL(void)			fks_free(void* ptr);

#ifdef __cplusplus
}
#endif
#endif


#endif	// FKS_MALLOC_H_INCLUDED
