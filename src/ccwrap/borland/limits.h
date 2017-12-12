#ifndef __CWRAP_LIMITS_H
#define __CWRAP_LIMITS_H

#include <ccwrap_header.h>
#include <stddef.h>

#if __BORLANDC__ >= 0x700
#include <../crtl/limits.h>
#else
#include __CCWRAP_NATIVE_C_HEADER_PATH(limits.h)
#if __BORLANDC__ < 0x560
#define LLONG_MIN   	    (-9223372036854775807-1)
#define LLONG_MAX   	      9223372036854775807
#define ULLONG_MAX  	      0xFFFFFFFFFFFFFFFF
#endif
#endif

#endif
