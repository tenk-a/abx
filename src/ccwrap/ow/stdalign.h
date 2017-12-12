/**
 *  @file   stdalign.h
 *  @brief  Alignment
 */
#pragma once

#include <ccwrap_header.h>

#if defined(_Alignas) // || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112)
#define alignas(a)              _Alignas(a)
#define __alignas_is_defined    1
#endif

#if defined(_Alignof) // || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112)
#define alignof(t)              _Alignof(t)
#define __alignof_is_defined    1
#endif
