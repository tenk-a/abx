/**
 *  @file   stdnoreturn.h
 *  @brief  _Noreturn   (B.22)
 */
#ifndef __CWRAP_STDNORETURN_H
#define __CWRAP_STDNORETURN_H

#include <ccwrap_header.h>

#ifndef __CCWRAP_NO_HEADER_STDNORETURN

#if __BORLANDC__ >= 0x700
#include <../crtl/stdbool.h>
#else
#include __CCWRAP_NATIVE_C_HEADER_PATH(stdbool.h)
#endif

#elif !defined(__cplusplus)

#if defined(_Noreturn) || (__BORLANDC__ >= 0x700) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
 #define noreturn                   _Noreturn
 #define __noreturn_is_defined      1
#else
 #error not impliment stdnoreturn.h
#endif

#endif

#endif
