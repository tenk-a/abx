#pragma once

#include <ccwrap_header.h>

#include __CCWRAP_NATIVE_C_HEADER_PATH(stdbool.h)

#ifndef __cplusplus
 #ifndef bool
  #if defined(_Bool)
   #define bool		_Bool
  #else
   #error not impliment stdbool.h
  #endif
 #endif
#endif
