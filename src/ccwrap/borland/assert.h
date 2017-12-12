#ifndef __CCWRAP_ASSERT_H
#define __CCWRAP_ASSERT_H

#include <ccwrap_header.h>

#if __BORLANDC__ >= 0x700

 #include <../crtl/assert.h>

#else

 #include __CCWRAP_NATIVE_C_HEADER_PATH(assert.h)

#endif

#ifndef __cplusplus
  #if 1 //__BORLANDC__ < 0x620/*?*/
   #if __BORLANDC__ >= 0x561
    #define static_assert(c,...)	_Static_assert(c,__VA_ARGS__)
   #else
    #define static_assert(c,m)		_Static_assert(c,m)
   #endif
  #endif
#endif

#endif
