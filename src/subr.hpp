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
#include <stdio.h>

#define STREND(p)   	((p)+strlen(p))
#define STPCPY(d,s) 	(strcpy((d),(s))+strlen(s)) 	/* strlen(d)ÇæÇ∆ï]âøèáÇ…ÇÊÇ¡ÇƒÇÕïsñ°Ç¢ÇÃÇæÇ¡ÇΩ... */

char const* StrSkipSpc(char const* s);
char const* StrSkipNotSpc(char const* s);

//enum { FIL_NMSZ = 2052 };
enum { FIL_NMSZ = 1024 + 16 };

#endif	/* SUBR_H */
