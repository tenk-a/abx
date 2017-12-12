#ifndef __CCWRAP_ERRNO_H
#define __CCWRAP_ERRNO_H

#pragma once

#include <ccwrap_header.h>
//#include <stddef.h>

#if __BORLANDC__ >= 0x700
#include <../crtl/errno.h>
#else
#include __CCWRAP_NATIVE_C_HEADER_PATH(errno.h)
#endif

#ifndef errno
#define errno		errno
#endif

#endif
