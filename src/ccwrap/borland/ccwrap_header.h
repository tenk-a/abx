/**
 *  @file ccwrap_header.h
 *  @brief for borland/Embarcadero free c/c++ compiler
 */
#ifndef __CCWRAP_HEADER_H
#define __CCWRAP_HEADER_H

#if !defined(__BORLANDC__)
#error this header is for borland-c++ (ver.5.5.1 or ver.7.20)
#endif

#define __CODEGUARD__
#undef _NO_INLINING

#ifndef __CCWRAP_M_CAT
  #define __CCWRAP_M_CAT(a,b) 		__CCWRAP_M_CAT_S2(a,b)
  #define __CCWRAP_M_CAT_S2(a,b)	__CCWRAP_M_CAT_S3(a##b)
  #define __CCWRAP_M_CAT_S3(x)		x
#endif

#if __BORLANDC__ < 0x700
#ifndef __CCWRAP_NATIVE_C_HEADER_DIR
#define __CCWRAP_NATIVE_C_HEADER_DIR	../include
#endif

#ifndef __CCWRAP_NATIVE_C_HEADER_PATH
#define __CCWRAP_NATIVE_C_HEADER_PATH(x)   <__CCWRAP_NATIVE_C_HEADER_DIR/##x>
#endif

#ifndef __CCWRAP_NATIVE_STL_HEADER_PATH
#define __CCWRAP_NATIVE_STL_HEADER_PATH(x)   <__CCWRAP_NATIVE_C_HEADER_DIR/##x>
#endif
#else
//#define __CCWRAP_NATIVE_C_HEADER_DIR	../crtl
#endif

#define __CCWRAP_NO_ALIGNAS
#define __CCWRAP_NO_THREAD_LOCAL

#define __CCWRAP_NO_HEADER_STDALIGN
#define __CCWRAP_NO_HEADER_STDNORETURN

#if __BORLANDC__ < 0x700
  //#if !defined(__has_include)
  //	#define __has_include(x)	    0
  //#endif
  //#if !defined(__has_feature)
  //  #define __has_feature(x)	    	0
  //#endif
  #if __BORLANDC__ < 0x610
   #if BORLANDC__ < 0x561
    #define __CCWRAP_NO_HEADER_STDBOOL
    #if !defined(__FUNCTION__)
     #define __FUNCTION__			""
    #endif
    #if !defined(__func__)
     #define __func__	    	    ""
    #endif
   #endif
  #endif
#endif

#if defined(__cplusplus)
 #if __BORLANDC__ < 0x700
  #if !defined(nullptr)
    #define nullptr	  		  	    (0)
  #endif
  #if !defined(override)
    #define override
  #endif
  #if !defined(final)
    #define final
  #endif
  //#if !defined(thread_local)
  //  #define thread_local
  //#endif
  #if !defined(noexcept)
    #define noexcept	    	    	throw()
  #endif
  #if !defined(alignof)
	namespace __ccwrap_detail {
		template<class T> class __alignof {
			struct U { char a_; T b_; };
		public:
			//enum { value = int(ptrdiff_t(&((U const*)0)->b_)) };
			enum { value = int((char*)&const_cast<char&>(reinterpret_cast<char const volatile&>(((U const*)0)->b_))) };
		};
	}
    #define alignof(a)				(__ccwrap_detail::__alignof<a>::value)
  #endif
  //#if !defined(alignas)
  //  #define alignas(a)
  //#endif
  #if __BORLANDC__ < 0x610
   #define __CCWRAP_NO_CXX11_AUTO	1
   #define __CCWRAP_NO_DECLTYPE 	1
   typedef wchar_t 	    	    	char16_t;
   typedef unsigned	    	    	char32_t;
   #if !defined(static_assert)
 	namespace __ccwrap { template<int x> struct static_assert_check{}; }
	template <bool x> struct __static_assert_FAILED_;
	template <> struct __static_assert_FAILED_<true> { enum { value = 1 }; };
   #endif
   #if __BORLANDC__ >= 0x561
    #if !defined(static_assert)
	 #define static_assert(c, ...)	typedef __ccwrap::static_assert_check<sizeof(__static_assert_FAILED_<(c) != 0>)> __CCWRAP_M_CAT(__ccwrap_static_assert_L_, __LINE__)
    #endif
    #if !defined(_Pragma)
	 #define _Pragma(...)
    #endif
   #else //if BORLANDC__ < 0x561
    #define __CCWRAP_NO_CXX11_VARIADIC_MACROS	1
    #if !defined(static_assert)
	 #define static_assert(c, m)	typedef __ccwrap::static_assert_check<sizeof(__static_assert_FAILED_<(c) != 0>)> __CCWRAP_M_CAT(__ccwrap_static_assert_L_, __LINE__)
    #endif
   #endif
  #endif
 #endif
#else	// for c
 #if __BORLANDC__ < 0x700
  #if __BORLANDC__ < 0x610
   #if !defined inline
    #define inline  	    	    __inline
   #endif
   #if !defined restrict
    #define restrict
   #endif
   #if !defined _Bool
    #define _Bool   	    		char
   #endif
   #ifndef _Noreturn
    #define _Noreturn
   #endif
   //#ifndef _Thread_local
   // #define _Thread_local
   //#endif
   //#ifndef _Alignas
   // #define _Alignas(a)
   //#endif
   #if !defined(_Alignof)
    #define _Alignof(T)				((unsigned)(&((struct {char a; T t_;}*)(0))->t_))
   #endif
   #if __BORLANDC__ >= 0x561
    #if !defined(_Static_assert)
     #define _Static_assert(c,...)	typedef char __CCWRAP_M_CAT(__static_assert_failed_L,__LINE__)[(c) ? 1/*OK*/ : -1/*NG*/]
    #endif
    #if !defined(_Pragma)
	 #define _Pragma(...)
    #endif
   #else //if BORLANDC__ < 0x561
    #if !defined(static_assert)
     #define _Static_assert(c,m)	typedef char __CCWRAP_M_CAT(__static_assert_failed_L,__LINE__)[(c) ? 1/*OK*/ : -1/*NG*/]
    #endif
    //#ifndef __STDC_VERSION__
     //#define __STDC_VERSION__	199409L
    //#endif
   #endif
  #endif
  #define __STDC_NO_VLA__
  #define __STDC_NO_COMPLEX__
  #define __STDC_NO_ATOMICS__
  #define __STDC_NO_THREADS__
  #define __STDC_NO_ANALYZABLE__
 #endif
#endif


#endif	// CCWRAP_HEADER_H_INCLUDED
