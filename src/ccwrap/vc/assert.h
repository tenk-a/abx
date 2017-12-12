#pragma once

#include <ccwrap_header.h>

#include __CCWRAP_NATIVE_UC_HEADER_PATH(assert.h)

#ifndef __cplusplus
 #if __MSC_VER < 1900
  #if __MSC_VER >= 1400
   #define static_assert(c,...)	_Static_assert(c,__VA_ARGS__)
  #else
   #define static_assert(c,m)	_Static_assert(c,m)
  #endif
 #endif
#endif
