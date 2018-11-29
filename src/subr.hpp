/**
 *  @file   subr.hpp
 *  @brief  abx misc
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @license Boost Software License Version 1.0
 */
#ifndef SUBR_HPP_INCLUDED
#define SUBR_HPP_INCLUDED

#include <stddef.h>
#include <fks_common.h>
#include <stdio.h>
#include <algorithm>
#include <string>
#include <list>

#include "StrzBuf.hpp"

 //enum { FIL_NMSZ = 2052 };
enum { FIL_NMSZ = 1024 + 16 };

typedef std::list<std::string>	StrList;
typedef StrzBuf<FIL_NMSZ>   	FnameBuf;

#define STREND(p)   	((p)+strlen(p))
#define STPCPY(d,s) 	(strcpy((d),(s))+strlen(s))

char const* StrSkipSpc(char const* s);
char const* StrSkipNotSpc(char const* s);


enum { OBUFSIZ	= 0x80000 };	/* 定義ファイル等のサイズ   	    	*/
enum { FMTSIZ	= 0x80000 };	/* 定義ファイル等のサイズ   	    	*/

std::string utf8toDbs(char const* s);
std::string dbsToUtf8(char const* s);
#define UTF8toDBS(s)	utf8toDbs(s).c_str()
#define DBStoUTF8(s)	dbsToUtf8(s).c_str()

std::string utf8ToConStr(char const* utf8);
std::string dbsToConStr(char const* dbs);
#define UTF8toCONS(s)	utf8ToConStr(s).c_str()
#define DBStoCONS(s)	dbsToConStr(s).c_str()


#ifdef FKS_PRIF_NO_LL	// for borland c++ 5.5.1
#define PRIF_LLONG		long
#define PRIF_ULLONG		unsigned long
#define PRIF_LL			"l"
#else
#define PRIF_LLONG		FKS_LLONG
#define PRIF_ULLONG		FKS_ULLONG
#define PRIF_LL			FKS_PRIF_LL		// "ll" or "I64"
#endif

#endif	/* SUBR_H */
