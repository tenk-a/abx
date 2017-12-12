/**
 *  @file   stdnoreturn.h
 *  @brief  _Noreturn   (B.22)
 */
#pragma once

#include <ccwrap_header.h>

#ifndef __cplusplus
#if defined(_Noreturn) // || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
 #define noreturn                   _Noreturn
 #define __noreturn_is_defined      1
#else
 #error not impliment stdnoreturn.h
#endif
#endif
