#pragma once

#include <ccwrap_header.h>
#include <stddef.h>
#include __CCWRAP_NATIVE_C_HEADER_PATH(limits.h)

#if _MSC_VER < 1300
#define LLONG_MIN   	    (-9223372036854775807LL-1LL)
#define LLONG_MAX   	      9223372036854775807LL
#define ULLONG_MAX  	      0xFFFFFFFFFFFFFFFFULL
#endif
