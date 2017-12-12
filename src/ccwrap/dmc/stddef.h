#pragma once

#include <ccwrap_header.h>

#include <../include/stddef.h>

union __ccwrap_max_align_t {
	void*		_m_vp;
	long long	_m_ll;
	double		_m_d;
	long double	_m_ld;
};
typedef union __ccwrap_max_align_t max_align_t;
