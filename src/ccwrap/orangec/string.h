#ifndef __CCWRAP_STRING_H
#define __CCWRAP_STRING_H

#include <ccwrap_header.h>

#include __CCWRAP_NATIVE_C_HEADER_PATH(string.h)

#undef  strupr
#undef  strlwr
#undef  strcoll
#undef  strxfrm

#endif
