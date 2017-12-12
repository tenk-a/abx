#ifndef __CWRAP_WCHAR_H
#define __CWRAP_WCHAR_H

#include <ccwrap_header.h>

#if __BORLANDC__ >= 0x700
#include <../crtl/wchar.h>
#else
#include __CCWRAP_NATIVE_C_HEADER_PATH(wchar.h)
#endif

#endif /* __CWRAP_WCHAR_H */
