#ifndef __CCWRAP_STRING_H
#define __CCWRAP_STRING_H

#pragma once

#include <ccwrap_header.h>
//#include <stddef.h>

#if __BORLANDC__ >= 0x700
 #include <../crtl/string.h>
#else
 #include __CCWRAP_NATIVE_C_HEADER_PATH(string.h)
 #if defined(__cplusplus) && __BORLANDC__ < 0x561
  using std::strlen;
  using std::strspn;
 #endif
#endif

#undef strcmp

#if __BORLANDC__ < 0x700 /*?*/
static inline char* stpcpy(char* d, char const* s) { return _stpcpy(d, s); }
#endif

static inline int	strcasecmp(char const* l, char const* r) { return stricmp(l, r); }
static inline int	strncasecmp(char const* l, char const* r, size_t n) { return strnicmp(l, r, n); }
#if __BORLANDC__ >= 0x560
static inline int	wcscasecmp(wchar_t const* l, wchar_t const* r) { return _wcsicmp(l, r); }
static inline int	wcsncasecmp(wchar_t const* l, wchar_t const* r, size_t n) { return _wcsnicmp(l, r, n); }
#endif


#endif
