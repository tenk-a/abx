#pragma once

#include <ccwrap_header.h>

#include __CCWRAP_NATIVE_C_HEADER_PATH(assert.h)

#if defined(_Static_assert) // || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
 #define static_assert		_Static_assert
#endif
