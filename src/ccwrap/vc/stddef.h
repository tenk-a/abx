#pragma once

#include <ccwrap_header.h>
#include __CCWRAP_NATIVE_UC_HEADER_PATH(stddef.h)

#if 1 //_MSC_VER < 1920
#if 0
union __ccwrap_max_align_t {
	__int64			i64_;
	long double 	ld_;
	alignas(8192)	__int64	a_;
	// _mm128 		mm128_;
};
typedef union __ccwrap_max_align_t	max_align_t;
#else
typedef double	max_align_t;
#endif
#endif
