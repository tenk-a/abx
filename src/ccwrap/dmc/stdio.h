#pragma once

#include <ccwrap_header.h>

#include <stddef.h>

#include <../include/stdio.h>


#include <stdarg.h>

static inline int	snprintf(char buf[], size_t bufsz, char const* fmt, ...) {
	int 	 n;
	va_list  a;
	va_start(a,fmt);
	n = _vsnprintf(buf, bufsz, fmt, a);
	buf[bufsz-1] = 0;
	va_end(a);
	return n;
}

static inline int	vsnwprintf(wchar_t buf[], size_t bufsz, wchar_t const* fmt, va_list a) {
	return _vsnwprintf(buf, bufsz, fmt, a);
}

static inline int	snwprintf(wchar_t buf[], size_t bufsz, wchar_t const* fmt, ...) {
	int 	 n;
	va_list  a;
	va_start(a,fmt);
	n = _vsnwprintf(buf, bufsz, fmt, a);
	buf[bufsz-1] = 0;
	va_end(a);
	return n;
}

