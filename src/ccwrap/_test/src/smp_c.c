#define __STDC_LIMIT_MACROS
// mingw Ç≈ stdio.h ÇÊÇËêÊÇ… stddef.h ÇincludeÇ∑ÇÈÇ∆
// __need_NULL,__need_size_t,__need_wchar_t,__need_wint_t Ç™ñ¢íËã`Ç≈wint_tÇ™Ç»Ç¢Ç»Ç«ÇÃÉGÉâÅ[Ç…ÇŒÇØÇƒÇµÇ‹Ç§...
#include <iso646.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>

#include <stdalign.h>
#include <stdbool.h>

static _Noreturn void  noreturnFunc1(void) {exit(0);}

#undef __STDBOOL_H
#include <stdnoreturn.h>

#if 0 //def getc
#error defined getc
#endif

_Static_assert(sizeof(int64_t) == 8, "");
_Static_assert(sizeof(double) >= 8, "");
_Static_assert(_Alignof(uint8_t) ==  1, "");
_Static_assert(_Alignof(uint16_t) ==  2, "");

static_assert(_Alignof(uint32_t) ==  4, "");
static_assert(_Alignof(uint64_t) ==  8, "");
static_assert(alignof(double) >=  8, "");


_Bool	b1 = false;
bool	b2 = true;

#ifndef __CCWRAP_NO_ALIGNAS
_Alignas(8)	int		aa1 = (int)_Alignof(bool);
alignas(8)	int 	aa2 = (int)alignof(int);
#endif

static noreturn void  noreturnFunc(void) {
	if (b1)
		noreturnFunc1();
	exit(0);
}


int main(int argc, char* argv[])
{
	char	buf[1024] = {0};
	char* 	p 		  = strdup("hello");
	assert(p != NULL);
	assert(strlen(p) == 5);
	assert(strcmp(p, "hello") == 0);
	//assert(strcasecmp(p, "HELLO") == 0);

	printf("\n%s\n", argv[0]);
	printf("%s (%d): %s -- __FILE__,__LINE__,__func__\n", __FILE__, __LINE__, __func__);
 #ifdef __STDC_VERSION__
	printf("\t__STDC_VERSION__ = %ld\n", __STDC_VERSION__);
 #endif
	printf("\tsizeof(_Bool) = %d\n", (int)sizeof(_Bool));
	printf("\tsizeof(wchar_t) = %d\n", (int)sizeof(wchar_t));
	printf("\tsizeof(intmax_t) = %2d  _Alignof(intmax_t) = %2d\n", (int)sizeof(intmax_t), (int)alignof(intmax_t));
	printf("\tsizeof(long double) = %2d  _Alignof(long double) = %2d\n", (int)sizeof(long double), (int)alignof(long double));
	printf("\talignof(max_align_t) = %d\n", (int)(_Alignof(max_align_t)));

	snprintf(buf, sizeof buf, "%s world! (%d)", p, (int)(strlen(p)));
	assert(strcmp(buf,"hello world! (5)") == 0);
	free(p);

 #ifdef PRId32
	snprintf(buf,  sizeof buf, "%" PRId32 "  %" PRIx32 "\n", INT32_C(1234567890), INT32_C(0x12345678));
	assert(strcmp(buf, "1234567890  12345678\n") == 0);
 #endif

 #ifdef PRId64
	snprintf(buf, sizeof buf, "%" PRId64 "  %" PRIx64 "\n", INT64_C(1234567890123456789), INT64_C(0x123456789abcdef0));
	assert(strcmp(buf, "1234567890123456789  123456789abcdef0\n") == 0);
 #endif

 #ifndef __CCWRAP_NO_ALIGNAS
	if (((uintptr_t)(&aa1) & 7) != 0) printf("bad _Alignas(8) aa1\n");
	if (((uintptr_t)(&aa2) & 7) != 0) printf("bad alignas(8) aa2\n");
 #endif

	printf("\n");

	noreturnFunc();
	return 0;
}
