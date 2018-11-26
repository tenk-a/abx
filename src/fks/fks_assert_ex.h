/**
 *	@file	fks_assert_ex.h
 *	@brief	ex assertion macro for fks-library.
 *  @author Masashi Kitamura (tenka@6809.net)
 *	@date	2000-2011
 *	@license Boost Software License Version 1.0
 */
#ifndef FKS_ASSERT_EX_H
#define FKS_ASSERT_EX_H

#include <fks/fks_config.h>

#ifdef FKS_HAS_PRAGMA_ONCE
 #pragma once
#endif


#ifdef NDEBUG
//#define FKS_ABORT_PRINTF(...)
#define FKS_ABORT_PRINTF(x)
#ifndef FKS_ASSERT
#define FKS_ASSERT(x)
#endif
#define FKS_PTR_ASSERT(p)
#define FKS_PTR0_ASSERT(p)
#define FKS_RANGE_ASSERT(x,a,b)
#define FKS_RANGE_PTR_ASSERT(x,a,b)
#define FKS_RANGE_I_ASSERT(x,a,b)
#define FKS_RANGE_L_ASSERT(x,a,b)
#define FKS_RANGE_LL_ASSERT(x,a,b)
#define FKS_RANGE_I64_ASSERT(x,a,b)
#define FKS_RANGE_U_ASSERT(x,a,b)
#define FKS_RANGE_UL_ASSERT(x,a,b)
#define FKS_RANGE_ULL_ASSERT(x,a,b)
#define FKS_RANGE_U64_ASSERT(x,a,b)
#define FKS_RANGE_F_ASSERT(x,a,b)
#define FKS_RANGE_LD_ASSERT(x,a,b)
#define FKS_RANGE_D_ASSERT(x,a,b)

#define FKS_RANGE_INTPTR_ASSERT(x,a,b)
#define FKS_RANGE_UINTPTR_ASSERT(x,a,b)

#define FKS_ARG_ASSERT(n,x)
#define FKS_ARG_PTR_ASSERT(n,x)
#define FKS_ARG_PTR0_ASSERT(n,x)
#define FKS_ARG_RANGE_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_PTR_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_I_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_L_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_LL_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_I64_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_U_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_UL_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_ULL_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_U64_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_F_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_LD_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_D_ASSERT(n,x,a,b)

#else	// NDEBUG

#ifndef FKS_ABORT_PRINTF
 #ifdef FKS_WIN32
  #ifdef __cplusplus
   extern "C" {
  #endif
    int fks_abort_printf(char const* fmt, ...) FKS_NOEXCEPT;
  #ifdef __cplusplus
   }
  #endif
  //#define FKS_ABORT_PRINTF(...)	fks_abort_printf(__VA_ARGS__)
  #define FKS_ABORT_PRINTF(x)    	fks_abort_printf x
 #else
  #include <stdio.h>
  //#define FKS_ABORT_PRINTF(...)	fprintf(stderr, __VA_ARGS__)
  #define FKS_ABORT_PRINTF(x)    	printf x
 #endif
#endif

#ifndef FKS_ASSERT
#define FKS_ASSERT(x)														\
		((x) || FKS_ABORT_PRINTF(("%s (%d) %s: assert(%s) is failed.\n"		\
				,__FILE__,__LINE__,__func__,#x)) )
#endif

#define FKS_PTR_ASSERT(p)													\
		(FKS_IS_RAM_PTR(p)													\
		 || FKS_ABORT_PRINTF(("%s (%d) %s: %s(0x%p) is bad pointer.\n"		\
				, __FILE__,__LINE__,__func__,#p, (p) )) )

#define FKS_PTR0_ASSERT(p)													\
		(!p || FKS_IS_RAM_PTR(p)											\
		 || FKS_ABORT_PRINTF(("%s (%d) %s: %s(0x%p) is bad pointer.\n"		\
		 		,__FILE__,__LINE__,__func__,#p, (p) )) )

#define FKS_RANGE_ASSERT(x,a,b)												\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF(("%s (%d) %s: %s, out of range[%s, %s].\n"		\
		 		,__FILE__,__LINE__,__func__,#x, #a, #b)) )

#define FKS_RANGE_PTR_ASSERT(x,a,b)											\
		( ((FKS_UINTPTR)(a) <= (FKS_UINTPTR)(x)								\
		    && (FKS_UINTPTR)(x) <= (FKS_UINTPTR)(b))						\
		 || FKS_ABORT_PRINTF((												\
		 		"%s (%d) %s: %s (%p), out of range[%p, %p].\n"				\
		 		,__FILE__,__LINE__,__func__,#x, (void*)(x)					\
		 		, (void*)(a), (void*)(b) )) )

#define FKS_RANGE_I_ASSERT(x,a,b)											\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF((												\
		 		"%s (%d) %s: %s (%d), out of range[%d, %d].\n"				\
		 		,__FILE__,__LINE__,__func__,#x,(int)(x)						\
		 		, (int)(a),(int)(b) )) )

#define FKS_RANGE_L_ASSERT(x,a,b)											\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF((												\
		 		"%s (%d) %s: %s (%ld), out of range[%ld, %ld].\n"			\
		 		, __FILE__,__LINE__,__func__,#x, (long)(x)					\
		 		, (long)(a), (long)(b) )) )

#define FKS_RANGE_LL_ASSERT(x,a,b)											\
		(((a) <= (x) && (x) <= (b))											\
		 || FKS_ABORT_PRINTF((												\
		 		 "%s (%d) %s: %s (%lld), out of range[%lld, %lld].\n"		\
		 		,__FILE__,__LINE__,__func__,#x, (FKS_LLONG)(x)				\
		 		, (FKS_LLONG)(a), (FKS_LLONG)(b)) ))

#define FKS_RANGE_U_ASSERT(x,a,b)											\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF(("%s (%d) %s: %s, out of range[%#x, %#x].\n"	\
			 	,__FILE__,__LINE__,__func__,#x, (unsigned)(x)				\
			 	, (unsigned)(a), (unsigned)(b) )) )

#define FKS_RANGE_UL_ASSERT(x,a,b)											\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF(("%s (%d) %s: %s, out of range[%#lx, %#lx].\n"	\
			 	,__FILE__,__LINE__,__func__,#x, (FKS_ULONG)(x)				\
			 	, (FKS_ULONG)(a), (FKS_ULONG)(b))))

#define FKS_RANGE_ULL_ASSERT(x,a,b)											\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF((												\
			 	 "%s (%d) %s: %s (%llx), out of range[%#llx, %#llx].\n"		\
			 	,__FILE__,__LINE__,__func__,#x, (FKS_ULLONG)(x)				\
			 	, (FKS_ULLONG)(a), (FKS_ULLONG)(b) )) )

#define FKS_RANGE_F_ASSERT(x,a,b)											\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF((												\
			 	 "%s (%d) %s: %s (%g), out of range[%g, %g].\n"				\
			 	,__FILE__,__LINE__,__func__,#x,(float)(x)					\
			 	, (float)(a), (float)(b) )) )

#define FKS_RANGE_D_ASSERT(x,a,b)											\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF((												\
		 		"%s (%d) %s: %s (%g), out of range[%g, %g].\n"				\
			 	,__FILE__,__LINE__,__func__,#x,(double)(x)					\
			 	, (double)(a), (double)(b) )) )

#define FKS_RANGE_LD_ASSERT(x,a,b)											\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF((												\
			 	 "%s (%d) %s: %s (%lg), out of range[%lg, %lg].\n"			\
			 	,__FILE__,__LINE__,__func__,#x, (long double)(x)			\
			 	, (long double)(a), (long double)(b) )) )

#if FKS_PTR_BIT == FKS_INT_BIT
#define FKS_RANGE_INTPTR_ASSERT(x,a,b) 		FKS_RANGE_I_ASSERT(x,a,b)
#define FKS_RANGE_UINTPTR_ASSERT(x,a,b)		FKS_RANGE_U_ASSERT(x,a,b)
#elif FKS_PTR_BIT == FKS_LONG_BIT
#define FKS_RANGE_INTPTR_ASSERT(x,a,b) 		FKS_RANGE_L_ASSERT(x,a,b)
#define FKS_RANGE_UINTPTR_ASSERT(x,a,b)		FKS_RANGE_UL_ASSERT(x,a,b)
#elif FKS_PTR_BIT == FKS_LLONG_BIT
#define FKS_RANGE_INTPTR_ASSERT(x,a,b) 		FKS_RANGE_LL_ASSERT(x,a,b)
#define FKS_RANGE_UINTPTR_ASSERT(x,a,b)		FKS_RANGE_ULL_ASSERT(x,a,b)
#endif
#if FKS_LONG_BIT == 64
#define FKS_RANGE_I64_ASSERT(n,x,a,b)		FKS_RANGE_L_ASSERT(x,a,b)
#define FKS_RANGE_U64_ASSERT(n,x,a,b)		FKS_RANGE_UL_ASSERT(x,a,b)
#elif FKS_LLONG_BIT == 64
#define FKS_RANGE_I64_ASSERT(n,x,a,b)		FKS_RANGE_LL_ASSERT(x,a,b)
#define FKS_RANGE_U64_ASSERT(n,x,a,b)		FKS_RANGE_ULL_ASSERT(x,a,b)
#endif


#define FKS_ARG_ASSERT(n,x)													\
		((x) || FKS_ABORT_PRINTF((											\
			 	"%s (%d) %s(arg %d): assert(%s) is failed.\n"				\
			 	,__FILE__,__LINE__,__func__,(n),#x )) )

#define FKS_ARG_PTR_ASSERT(n,p)												\
		( FKS_IS_RAM_PTR(p)													\
			|| FKS_ABORT_PRINTF((											\
				"%s (%d) %s(arg %d): %s(0x%p) is bad pointer.\n"			\
				,__FILE__,__LINE__,__func__,(n), #p, (p) )) )

#define FKS_ARG_PTR0_ASSERT(n,p)											\
		(!(p) || FKS_IS_RAM_PTR(p)											\
		 || FKS_ABORT_PRINTF((												\
		  		"%s (%d) %s(arg %d): %s(0x%p) is bad pointer.\n"			\
		 		,__FILE__,__LINE__,__func__,(n), #p, (p) )) )

#define FKS_ARG_RANGE_ASSERT(n,x,a,b)										\
		(!((a) <= (x) && (x) <= (b))										\
		 && FKS_ABORT_PRINTF((												\
		 		"%s (%d) %s(arg %d): %s, out of range[%s, %s].\n"			\
		 		,__FILE__,__LINE__,__func__,(n), #x, #a, #b )) )

#define FKS_ARG_RANGE_PTR_ASSERT(n,x,a,b)									\
		( ((FKS_UINTPTR)(a) <= (FKS_UINTPTR)(x)								\
		    && (FKS_UINTPTR)(x) <= (FKS_UINTPTR)(b))						\
		 || FKS_ABORT_PRINTF((												\
		 		"%s (%d) %s: %s (%p), out of range[%p, %p].\n"				\
		 		,__FILE__,__LINE__,__func__,(n), #x, (void*)(x)				\
		 		, (void*)(a), (void*)(b) )) )

#define FKS_ARG_RANGE_I_ASSERT(n,x,a,b)										\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF((												\
		 		"%s (%d) %s: %s (%d), out of range[%d, %d].\n"				\
		 		,__FILE__,__LINE__,__func__,(n),#x,(int)(x)					\
		 		, (int)(a),(int)(b) )) )

#define FKS_ARG_RANGE_L_ASSERT(n,x,a,b)										\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF((												\
			 	 "%s (%d) %s: %s (%ld), out of range[%ld, %ld].\n"			\
			 	,__FILE__,__LINE__,__func__,(n),#x, (long)(x)				\
			 	, (long)(a), (long)(b) )) )

#define FKS_ARG_RANGE_LL_ASSERT(n,x,a,b)									\
		(((a) <= (x) && (x) <= (b))											\
		 || FKS_ABORT_PRINTF((												\
			 	 "%s (%d) %s: %s (%lld), out of range[%lld, %lld].\n"		\
			 	,__FILE__,__LINE__,__func__,(n),#x, (FKS_LLONG)(x)			\
			 	, (FKS_LLONG)(a), (FKS_LLONG)(b)) ))

#define FKS_ARG_RANGE_UI_ASSERT(n,x,a,b)									\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF(("%s (%d) %s: %s, out of range[%#x, %#x].\n"	\
			 	,__FILE__,__LINE__,__func__,(n),#x, (unsigned)(x)			\
			 	, (unsigned)(a), (unsigned)(b) )) )

#define FKS_ARG_RANGE_UL_ASSERT(n,x,a,b)									\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF(("%s (%d) %s: %s, out of range[%#lx, %#lx].\n"	\
			 	,__FILE__,__LINE__,__func__,(n),#x, (FKS_ULONG)(x)			\
			 	, (FKS_ULONG)(a), (FKS_ULONG)(b))))

#define FKS_ARG_RANGE_ULL_ASSERT(n,x,a,b)									\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF((												\
			 	 "%s (%d) %s: %s (%llx), out of range[%#llx, %#llx].\n"		\
			 	,__FILE__,__LINE__,__func__,(n),#x, (FKS_ULLONG)(x)			\
			 	, (FKS_ULLONG)(a), (FKS_ULLONG)(b) )) )

#if FKS_PTR_BIT == FKS_INT_BIT
#define FKS_ARG_RANGE_INTPTR_ASSERT(n,x,a,b)  FKS_ARG_RANGE_I_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_UINTPTR_ASSERT(n,x,a,b) FKS_ARG_RANGE_U_ASSERT(n,x,a,b)
#elif FKS_PTR_BIT == FKS_LONG_BIT
#define FKS_ARG_RANGE_INTPTR_ASSERT(n,x,a,b)  FKS_ARG_RANGE_L_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_UINTPTR_ASSERT(n,x,a,b) FKS_ARG_RANGE_UL_ASSERT(n,x,a,b)
#elif FKS_PTR_BIT == FKS_LLONG_BIT
#define FKS_ARG_RANGE_INTPTR_ASSERT(n,x,a,b)  FKS_ARG_RANGE_LL_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_UINTPTR_ASSERT(n,x,a,b) FKS_ARG_RANGE_ULL_ASSERT(n,x,a,b)
#endif
#if FKS_LONG_BIT == 64
#define FKS_ARG_RANGE_I64_ASSERT(n,x,a,b)	FKS_ARG_RANGE_L_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_U64_ASSERT(n,x,a,b)	FKS_ARG_RANGE_UL_ASSERT(n,x,a,b)
#elif FKS_LLONG_BIT == 64
#define FKS_ARG_RANGE_I64_ASSERT(n,x,a,b)	FKS_ARG_RANGE_LL_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_U64_ASSERT(n,x,a,b)	FKS_ARG_RANGE_ULL_ASSERT(n,x,a,b)
#endif

#define FKS_ARG_RANGE_F_ASSERT(n,x,a,b)										\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF((												\
			 	 "%s (%d) %s: %s (%g), out of range[%g, %g].\n"				\
			 	,__FILE__,__LINE__,__func__,(n),#x,(float)(x)				\
			 	, (float)(a), (float)(b) )) )

#define FKS_ARG_RANGE_D_ASSERT(n,x,a,b)										\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF((												\
			 	"%s (%d) %s: %s (%g), out of range[%g, %g].\n"				\
			 	,__FILE__,__LINE__,__func__,(n),#x,(double)(x)				\
			 	, (double)(a), (double)(b) )) )

#define FKS_ARG_RANGE_LD_ASSERT(n,x,a,b)									\
		( ((a) <= (x) && (x) <= (b))										\
		 || FKS_ABORT_PRINTF((												\
			 	 "%s (%d) %s: %s (%lg), out of range[%lg, %lg].\n"			\
			 	,__FILE__,__LINE__,__func__,(n),#x, (long double)(x)		\
			 	, (long double)(a), (long double)(b) )) )

#endif	// NDEBUG



#endif	// FKS_ASSERT_H_INCLUDED
