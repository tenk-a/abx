/**
 *  @file   stdbool.h
 *  @brief  Boolean type and values
 */
#ifndef __CWRAP_STDBOOL_H
#define __CWRAP_STDBOOL_H

#include <ccwrap_header.h>

#ifndef __CCWRAP_NO_HEADER_STDBOOL

#if __BORLANDC__ >= 0x700
#include <../crtl/stdbool.h>
#else
#include __CCWRAP_NATIVE_C_HEADER_PATH(stdbool.h)
#endif

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

#endif
