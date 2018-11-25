/**
 *  @file   subr.h
 *  @brief
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @note
 *  	Boost Software License Version 1.0
 */
#ifndef SUBR_HPP_INCLUDED
#define SUBR_HPP_INCLUDED

#include <stddef.h>
#include <fks/fks_config.h>
#include <stdio.h>
#include <algorithm>
#include <string>

#define STREND(p)   	((p)+strlen(p))
#define STPCPY(d,s) 	(strcpy((d),(s))+strlen(s))

char const* StrSkipSpc(char const* s);
char const* StrSkipNotSpc(char const* s);

//enum { FIL_NMSZ = 2052 };
enum { FIL_NMSZ = 1024 + 16 };


std::string utf8toDbs(char const* s);
std::string dbsToUtf8(char const* s);
#define UTF8toDBS(s)	utf8toDbs(s).c_str()
#define DBStoUTF8(s)	dbsToUtf8(s).c_str()

std::string utf8ToConStr(char const* utf8);
std::string dbsToConStr(char const* dbs);
#define UTF8toCONS(s)	utf8ToConStr(s).c_str()
#define DBStoCONS(s)	dbsToConStr(s).c_str()


#endif	/* SUBR_H */
