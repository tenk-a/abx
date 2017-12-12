#ifndef __CCWRAP_STDDEF_H
#define __CCWRAP_STDDEF_H

#pragma once

#include <ccwrap_header.h>

#if __BORLANDC__ >= 0x700
#include <../crtl/stddef.h>
#else
#include __CCWRAP_NATIVE_C_HEADER_PATH(stddef.h)
#if 1
struct __ccwrap_max_align_t {
	__int64		ll_;
	double		d_;
	long double ld_;
};
typedef struct __ccwrap_max_align_t	max_align_t;
#else
typedef long double	max_align_t;
#endif
#endif

#endif
