/**
 *  @file   fks_misc.c
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks_common.h>
#include <stddef.h>
#include <string.h>

#ifdef FKS_WIN32
#include "msw/fks_misc_w32.hh"
#elif 1 //defined FKS_LINUX // || defined FKS_BSD
//#include "uni/fks_misc_uni.hh"
#endif

#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL (char const*)
fks_skipSpc(char const* s)
{
    while (*s && *(unsigned char *)s <= ' ')
        s++;
    return s;
}


FKS_LIB_DECL (char const*)
fks_skipNotSpc(char const* s)
{
    while (*(unsigned char *)s > ' ')
        s++;
    return s;
}



/// @param flags : bit0:'\n' bit1:'\r' bit2='\r\n' bit3:'\0'
FKS_LIB_DECL (char const*)
fks_strGetLine(char line[], size_t lineSz, char const* src, char const* srcEnd, size_t flags)
{
	char const* s = src;
	char const* e;
	char const* nxt;
	size_t		l;
	if (!s)
		return NULL;
	if (s >= srcEnd)
		return NULL;
	nxt = srcEnd;
	do {
		unsigned c = *s++;
		if (c == '\n' && (flags & 1)) {
			nxt = s;
			break;
		} else if (c == '\r') {
			if (*s == '\n' && (flags & 4)) {
				nxt = s + 1;
				break;
			} else if (flags & 2) {
				nxt = s;
				break;
			}
		} else if (c == 0 && (flags & 8)) {
			nxt = s;
			break;
		}
	} while (s < srcEnd);
	l = s - src;
	if (l > 0) {
		if (l > lineSz-1)
			l = lineSz-1;
		memcpy(&line[0], src, l);
		line[l] = 0;
		return nxt;
	}
	return NULL;
}


#ifdef __cplusplus
}
#endif
