/**
 *  @file   subr.hpp
 *  @brief  abx misc
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @license Boost Software License Version 1.0
 */
#ifndef SUBR_HPP_INCLUDED
#define SUBR_HPP_INCLUDED

#include <stddef.h>
#include <fks/fks_common.h>
#include <stdio.h>
#include <algorithm>
#include <string>
#include <list>
#include <stdint.h>

#include "StrzBuf.hpp"

enum { FPATH_SIZE = 260*6 + 504 };

typedef std::list<std::string>	StrList;
typedef StrzBuf<FPATH_SIZE>		FPathBuf;

#define STREND(p)   	((p)+strlen(p))

enum { OBUFSIZ	= 0x100000 };
enum { FMTSIZ	= 0x100000 };

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
