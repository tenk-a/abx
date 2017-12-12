#ifndef __CCWRAP_STDIO_H
#define __CCWRAP_STDIO_H

#include <ccwrap_header.h>

#include __CCWRAP_NATIVE_C_HEADER_PATH(stdio.h)

#undef getc
#undef putc
#undef getchar
#undef putchar
#undef ungetc

#endif
