/**
 *  @file   subr.cpp
 *  @brief  subrutine
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @note
 *  	Boost Software License Version 1.0
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#include "subr.hpp"
#include <fks/fks_alloca.h>

#ifdef _WIN32
#include <windows.h>
#endif



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


#ifdef _WIN32

std::string utf8toDbs(char const* s)
{
	size_t   sl = strlen(s) + 1;
	size_t   bl = MultiByteToWideChar(65001,0,s,sl,NULL,0) + 1;
	wchar_t* b  = (wchar_t*)fks_alloca(bl*2);
	MultiByteToWideChar(65001,0,s,sl,b,bl*2);
	size_t   dl = WideCharToMultiByte(0,0,b,bl,NULL,0,0,0) + 1;
	char*    d  = (char*)fks_alloca(dl);
	WideCharToMultiByte(0,0,b,bl,d,dl,0,0);
	return std::string(d);
}

std::string dbsToUtf8(char const* s)
{
	size_t   sl = strlen(s) + 1;
	size_t   bl = MultiByteToWideChar(0,0,s,sl,NULL,0) + 1;
	wchar_t* b  = (wchar_t*)fks_alloca(bl*2);
	MultiByteToWideChar(0,0,s,sl,b,bl*2);
	size_t   dl = WideCharToMultiByte(65001,0,b,bl,NULL,0,0,0) + 1;
	char*    d  = (char*)fks_alloca(dl);
	WideCharToMultiByte(65001,0,b,bl,d,dl,0,0);
	return std::string(d);
}

std::string utf8ToConStr(char const* utf8)
{
	int cp = GetConsoleOutputCP();
	if (cp == 65001)
		return utf8;
	return utf8toDbs(utf8);
}

std::string dbsToConStr(char const* dbs)
{
	int cp = GetConsoleOutputCP();
	if (cp == 65001)
		return dbsToUtf8(dbs);
	return dbs;
}

#endif
