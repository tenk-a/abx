#pragma once

#include <ccwrap_header.h>
#include <stddef.h>

#include <../include/string.h>

#if 1
inline char*	strdup(char const* s) { return _strdup(s); }
//inline char* strupr(char* p) { return _strupr(s); }
//inline char* strlwr(char* p) { return _strlwr(s); }

inline int	strcasecmp(char const* l, char const* r) { return _stricmp(l, r); }
inline int	strncasecmp(char const* l, char const* r, size_t n) { return _strnicmp(l, r, n); }
inline int	wcscasecmp(wchar_t const* l, wchar_t const* r) { return _wcsicmp(l, r); }
inline int	wcsncasecmp(wchar_t const* l, wchar_t const* r, size_t n) { return _wcsnicmp(l, r, n); }
#endif
