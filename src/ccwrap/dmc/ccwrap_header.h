/**
 *  @file ccwrap_header.h
 *  @brief for digitalmars c/c++
 */
#pragma once

#if !defined(__DMC__)
#error this header is for dmc
#endif

#ifndef __CCWRAP_M_CAT
  #define __CCWRAP_M_CAT(a,b) 		__CCWRAP_M_CAT_S2(a,b)
  #define __CCWRAP_M_CAT_S2(a,b)	__CCWRAP_M_CAT_S3(a##b)
  #define __CCWRAP_M_CAT_S3(x)		x
#endif


#define __CCWRAP_NO_ALIGNAS
#define __CCWRAP_NO_THREAD_LOCAL

//#if !defined(__has_include)
//  #define __has_include(x)	   		0
//#endif
//#if !defined(__has_feature)
//  #define __has_feature(x)	    	0
//#endif
#if !defined(_Pragma)
  #define _Pragma(...)
#endif


#if defined(__cplusplus)
  #define __CCWRAP_NO_CXX11_AUTO	1
  #define __CCWRAP_NO_DECLTYPE 		1
  #if !defined(noexcept)
    #define noexcept	    	    throw()
  #endif
  #if !defined(alignof)
	namespace __ccwrap_detail {
		template<class T> class __alignof {
			struct U { char a; T b; };
			enum { nt = sizeof(T), nu = sizeof(U), d = nu - nt };
		public:
			enum { value = (nt < d) ? nt : d };
		};
	}
    #define alignof(a)				(__ccwrap_detail::__alignof<a>::value)
  #endif
  //#if !defined(alignas)
  //  #define alignas(a)
  //#endif
  #if !defined(override)
    #define override
  #endif
  #if !defined(final)
    #define final
  #endif
  //#if !defined(thread_local)
  //  #define thread_local
  //#endif
#else	// for c
  #if !defined(_Alignof)
    #define _Alignof(type)			((size_t)((ptrdiff_t)(&((struct {char a_; type t_;}*)(0))->t_)))
  #endif
  //#ifndef _Alignas
  //  #define _Alignas(a)
  //#endif
  #ifndef _Noreturn
    #define _Noreturn
  #endif
  //#ifndef _Thread_local
  //  #define _Thread_local
  //#endif
  #ifndef _Static_assert
    #define _Static_assert(c,...)	typedef char __CCWRAP_M_CAT(__Static_assert_FAILED_L,__LINE__)[(c) ? 1/*OK*/ : -1/*NG*/]
  #endif
#endif
