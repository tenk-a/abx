/**
 *  @file   stdalign.h
 *  @brief  Alignment
 */
#ifndef __CWRAP_STDALIGN_H
#define __CWRAP_STDALIGN_H

#include <ccwrap_header.h>

#ifndef __CCWRAP_NO_HEADER_STDALIGN

#if __BORLANDC__ >= 0x700
#include <../crtl/stdalign.h>
#else
#include __CCWRAP_NATIVE_C_HEADER_PATH(stdalign.h)
#endif

#elif !defined(__cplusplus)

#if defined(_Alignas) || (__BORLANDC__ >= 0x700) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112)
#define alignas(a)              _Alignas(a)
#define __alignas_is_defined    1
#endif

#if defined(_Alignof) || (__BORLANDC__ >= 0x700) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112)
#define alignof(t)              _Alignof(t)
#define __alignof_is_defined    1
#endif

#endif

#endif
