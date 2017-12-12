#pragma once

#include <ccwrap_header.h>
#include <stddef.h>
#include __CCWRAP_NATIVE_UC_HEADER_PATH(stdio.h)

#if _MSC_VER < 1900

#include <stdarg.h>

__forceinline int	vsnprintf(char buf[], size_t bufsz, char const* fmt, va_list a) {
 #if _MSC_VER >= 1400
	return _vsnprintf(buf, bufsz, fmt, a);
 #else
	return vsprintf(buf, fmt, a);
 #endif
}

__forceinline int	snprintf(char buf[], size_t bufsz, char const* fmt, ...) {
	int 	 n;
	va_list  a;
	va_start(a,fmt);
 #if _MSC_VER >= 1400
	n = _vsnprintf(buf, bufsz, fmt, a);
 #else
	n = vsprintf(buf, fmt, a);
 #endif
	buf[bufsz-1] = 0;
	va_end(a);
	return n;
}

__forceinline int	vsnwprintf(wchar_t buf[], size_t bufsz, wchar_t const* fmt, va_list a) {
 #if _MSC_VER >= 1400
	return _vsnwprintf(buf, bufsz, fmt, a);
 #else
	return vswprintf(buf, fmt, a);
 #endif
}

__forceinline int	snwprintf(wchar_t buf[], size_t bufsz, wchar_t const* fmt, ...) {
	int 	 n;
	va_list  a;
	va_start(a,fmt);
 #if _MSC_VER >= 1400
	n = _vsnwprintf(buf, bufsz, fmt, a);
 #else
	n = vswprintf(buf, fmt, a);
 #endif
	buf[bufsz-1] = 0;
	va_end(a);
	return n;
}

#endif
