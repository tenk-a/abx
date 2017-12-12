/**
 *  @file   stdbool.h
 *  @brief  Boolean type and values
 */
#pragma once

#include <ccwrap_header.h>

#ifndef __CCWRAP_NO_HEADER_STDBOOL

#include __CCWRAP_NATIVE_C_HEADER_PATH(stdbool.h)

#elif !defined(__cplusplus)

#if defined(_Bool) // || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
 #define bool                   _Bool
 #define true                   1
 #define false                  0
 #define __bool_is_defined      1
#else
 #error not impliment stdbool.h
#endif

#endif
