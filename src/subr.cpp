/**
 *  @file   subr.cpp
 *  @brief  サブルーチン
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @note
 *  	Boost Software License Version 1.0
 */
#include "subr.hpp"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>



char const* StrSkipSpc(char const* s)
{
    while (*s && *(unsigned char *)s <= ' ')
    	s++;
    return s;
}

char const* StrSkipNotSpc(char const* s)
{
    while (*(unsigned char *)s > ' ')
    	s++;
    return s;
}
