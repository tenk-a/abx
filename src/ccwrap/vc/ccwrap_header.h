/**
 *  @file ccwrap_header.h
 *  @brief  c/c++ language
 *	@author	Masashi Kitamura
 */
#pragma once

/*
    6.0     6.0     1200
    2002    7.0     1300
    2003    7.1     1310
    2005    8.0     1400
    2008    9.0     1500
    2010    10.0    1600
    2012    11.0    1700
    2013    12.0    1800
    2015    14.0    1900
    2017    14.1    1910
*/

#if !defined(_MSC_VER)
#error for visual c/c++ header
#endif

#ifndef __CCWRAP_M_CAT
  #define __CCWRAP_M_CAT(a,b) 		__CCWRAP_M_CAT_S2(a,b)
  #define __CCWRAP_M_CAT_S2(a,b)	__CCWRAP_M_CAT_S3(a##b)
  #define __CCWRAP_M_CAT_S3(x)		x
#endif

#ifndef __CCWRAP_NATIVE_C_HEADER_DIR
  #if _MSC_VER >= 1910
    #define __CCWRAP_NATIVE_C_HEADER_DIR	../include
    #define __CCWRAP_NATIVE_UC_HEADER_DIR	../ucrt
  #elif _MSC_VER >= 1900
    #define __CCWRAP_NATIVE_C_HEADER_DIR	../../VC/include
    #define __CCWRAP_NATIVE_UC_HEADER_DIR	../ucrt
  #elif _MSC_VER >= 1400
    #define __CCWRAP_NATIVE_C_HEADER_DIR	../../VC/include
    #define __CCWRAP_NATIVE_UC_HEADER_DIR	../../VC/include
  #elif _MSC_VER >= 1300
    #define __CCWRAP_NATIVE_C_HEADER_DIR   	../../Vc7/include
    #define __CCWRAP_NATIVE_UC_HEADER_DIR	../../Vc7/include
  #elif _MSC_VER >= 1200
    #define __CCWRAP_NATIVE_C_HEADER_DIR   	../../VC6/include
    #define __CCWRAP_NATIVE_UC_HEADER_DIR	../../VC6/include
  #endif
#endif

#ifndef __CCWRAP_NATIVE_C_HEADER_PATH
  #define __CCWRAP_NATIVE_C_HEADER_PATH(x)   <__CCWRAP_NATIVE_C_HEADER_DIR/##x>
#endif
#ifndef __CCWRAP_NATIVE_UC_HEADER_PATH
  #define __CCWRAP_NATIVE_UC_HEADER_PATH(x)   <__CCWRAP_NATIVE_UC_HEADER_DIR/##x>
#endif
#ifndef __CCWRAP_NATIVE_STL_HEADER_PATH
  #define __CCWRAP_NATIVE_STL_HEADER_PATH(x)   <__CCWRAP_NATIVE_C_HEADER_DIR/##x>
#endif


#if _MSC_VER < 1910
  //#if !defined(__has_include)
  // #define __has_include(x)	    0
  //#endif
  //#if !defined(__has_feature)
  // #define __has_feature(x)	    0
  //#endif
  #define _XKEYCHECK_H
  #if _MSC_VER < 1900
    #if _MSC_VER < 1600
      #if _MSC_VER >= 1400
        #if !defined(_Pragma)
          #define _Pragma(...)   		__pragma(__VA_ARGS__)
        #endif
      #else
        #if !defined(_Pragma)
          #define _Pragma(x)   	    	__pragma(x)
        #endif
      #endif
    #endif
  #endif
#endif


#if defined(__cplusplus)
  #if _MSC_VER < 1910
    //#if _MSC_VER >= 1600
	 //#define static_assert(c, ...)	static_assert((c), "" __VA_ARGS__)
    //#endif
    #if _MSC_VER < 1900	// vc12 or less
      #if !defined(alignas)
        #define alignas(a)     		    __declspec(align(a))
      #endif
      #if !defined(alignof)
         #define alignof(a)     	    __alignof(a)
      #endif
      #if !defined(__func__)
        #define __func__	    	    __FUNCTION__
      #endif
      #if !defined(thread_local)
        #define thread_local   		    __declspec(thread)
      #endif
      #if _MSC_VER < 1700	// vc10 or less
        #if !defined(final) && _MSC_VER >= 1400	// vc8-10
          #define final  	    	    sealed
        #else
          #define final
        #endif
        #if _MSC_VER < 1600	// vc9 or less
          #if !defined(noexcept)
            #define noexcept	    	throw()
          #endif
          #if !defined(nullptr)
            #define nullptr	    		(0)
          #endif
          #if !defined(static_assert)
            namespace __ccwrap { template<int x> struct static_assert_check{}; }
            template <bool x> struct __static_assert_FAILED_;
            template <> struct __static_assert_FAILED_<true> { enum { value = 1 }; };
            #if _MSC_VER >= 1400	// vc8, vc9
              #define static_assert(c, ...)	typedef __ccwrap::static_assert_check<sizeof(__static_assert_FAILED_<(c) != 0>)> __CCWRAP_M_CAT(__ccwrap_static_assert_L_, __LINE__)
            #else	// vc7.1 or less
              #define static_assert(c, m)	typedef __ccwrap::static_assert_check<sizeof(__static_assert_FAILED_<(c) != 0>)> __CCWRAP_M_CAT(__ccwrap_static_assert_L_, __LINE__)
            #endif
          #endif
          #define __CCWRAP_NO_CXX11_AUTO	1
          #define __CCWRAP_NO_DECLTYPE 		1
          #define __CCWRAP_LLONG		__int64
          #define __CCWRAP_ULLONG		unsigned __int64
          #define __CCWRAP_PFMT_LL		"I64"
          #define __CCWRAP_PFMT_WLL		L"I64"
          typedef unsigned	    		char32_t;
          #if _MSC_VER >= 1400			// vc8, vc9
            typedef wchar_t 	   		char16_t;
          #else							// vc7.1 or less
            typedef unsigned short		char16_t;
    		  #define WCHAR_MAX 		((wchar_t)-1)
            #if !defined(override)
              #define override
            #endif
          #endif
        #endif
      #endif
    #endif
  #endif
  #if _MSC_VER < 1900	// vc12 or less
    #define __CCWRAP_NO_HEADER_CXX14_MEMORY             0
    #if _MSC_VER >= 1400
     #define __CCWRAP_NO_HEADER_SHARED_MUTEX			0
    #else
     #define __CCWRAP_NO_HEADER_SHARED_MUTEX			1
    #endif
    #if _MSC_VER < 1800	// vc11 or less
      #define __CCWRAP_NO_HEADER_CFENV					1
      #define __CCWRAP_NO_HEADER_INITIALIZER_LIST		1
      #if _MSC_VER < 1700	// vc10 or less
        #if _MSC_VER >= 1400
         #define __CCWRAP_NO_HEADER_THREAD				0
        #else
         #define __CCWRAP_NO_HEADER_THREAD				1
        #endif
        #define __CCWRAP_NO_HEADER_ATOMIC				1
        #define __CCWRAP_NO_HEADER_CHRONO				0
        #define __CCWRAP_NO_HEADER_CONDITION_VARIABLE	0
        #if _MSC_VER >= 1400
         #define __CCWRAP_NO_HEADER_FUTURE				0
         #define BOOST_THREAD_PROVIDES_FUTURE
         #define __CCWRAP_NO_HEADER_MUTEX				0
        #else
         #define __CCWRAP_NO_HEADER_FUTURE				1
         #define __CCWRAP_NO_HEADER_MUTEX				1
        #endif
        #define __CCWRAP_NO_HEADER_RATIO				0
        #define __CCWRAP_NO_HEADER_SCOPED_ALLOCATOR		0
        #if _MSC_VER < 1600 || (_MSC_VER == 1600 && defined __CCWRAP_NO_TR1)	// vc9 or less or no-TR1
          #define __CCWRAP_NO_HEADER_ARRAY				0
          #define __CCWRAP_NO_HEADER_FORWARD_LIST		0
          #define __CCWRAP_NO_HEADER_RANDOM				0
          #define __CCWRAP_NO_HEADER_REGEX				0
          #define __CCWRAP_NO_HEADER_TUPLE				0
          #define __CCWRAP_NO_HEADER_TYPE_TRAITS		0
          #define __CCWRAP_NO_HEADER_UNORDERED_MAP		0
          #define __CCWRAP_NO_HEADER_UNORDERED_SET		0
        #endif
        #if _MSC_VER < 1600	// vc9 or less
          #define __CCWRAP_NO_HEADER_CODECVT			1
          #define __CCWRAP_NO_HEADER_TYPEINDEX			0
          // for ccwrap/boost2std
          #define __CCWRAP_NO_CXX11_ALGORITHM			0
	      #define __CCWRAP_NO_CXX11_COMPLEX				0
	      #define __CCWRAP_NO_CXX11_FUNCTIONAL			0
	      #define __CCWRAP_NO_CXX11_IOMANIP				0
	      #define __CCWRAP_NO_CXX11_IOS					1
	      #define __CCWRAP_NO_CXX11_LIMITS				0
	      #define __CCWRAP_NO_CXX11_MEMORY				0
	      #define __CCWRAP_NO_CXX11_UTILITY				0
	      #define __CCWRAP_NO_CXX11_LOCALE				1
	      #define __CCWRAP_NO_CXX11_NUMERIC				0
	      #define __CCWRAP_NO_CXX11_STRING				0
	      #if _MSC_VER >= 1400
           #define __CCWRAP_NO_HEADER_SYSTEM_ERROR		0
	      #else
           #define __CCWRAP_NO_HEADER_SYSTEM_ERROR		1
           #define __CCWRAP_NO_CXX11_VARIADIC_MACROS	1
	      #endif
	    #endif
      #endif
    #endif
  #endif
#else	// for c
  #define __STDC_NO_VLA__
  #define __STDC_NO_COMPLEX__
  //#if !defined restrict	//BAD: vc's header used __declspec(restrict)
    //#define restrict	    	    __restrict	//__declspec(restrict)
  //#endif
  #ifndef _Thread_local
    #define _Thread_local   	    __declspec(thread)
  #endif
  #ifndef _Static_assert
   #if _MSC_VER >= 1400
    #define _Static_assert(c,...)	typedef char __CCWRAP_M_CAT(__static_assert_failed_L,__LINE__)[(c) ? 1/*OK*/ : -1/*NG*/]
   #else
    #define _Static_assert(c, m)	typedef char __CCWRAP_M_CAT(__static_assert_failed_L,__LINE__)[(c) ? 1/*OK*/ : -1/*NG*/]
   #endif
  #endif
  #if !defined(_Alignof)
    #define _Alignof(t)     	    __alignof(t)
  #endif
  #ifndef _Alignas
    #define _Alignas(a)     	    __declspec(align(a))
  #endif
  #ifndef _Noreturn
    #define _Noreturn	    	    __declspec(noreturn)
  #endif
  #if !defined(__func__)
   #define __func__	    	    	__FUNCTION__
  #endif
  #if _MSC_VER < 1900
   #define __CCWRAP_NO_HEADER_STDBOOL
   #if !defined inline
    #define inline  	    	    __inline
   #endif
   #if !defined _Bool
    #define _Bool   	    	    char
   #endif
   #if _MSC_VER < 1400
    #define WCHAR_MAX 				((wchar_t)-1)
   #endif
   //#ifndef __STDC_VERSION__
    //#define __STDC_VERSION__		199409L
   //#endif
  #endif
#endif
