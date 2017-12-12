#define __STDC_LIMIT_MACROS
// mingw Ç≈ stdio.h ÇÊÇËêÊÇ… stddef.h ÇincludeÇ∑ÇÈÇ∆
// __need_NULL,__need_size_t,__need_wchar_t,__need_wint_t Ç™ñ¢íËã`Ç≈wint_tÇ™Ç»Ç¢Ç»Ç«ÇÃÉGÉâÅ[Ç…ÇŒÇØÇƒÇµÇ‹Ç§...
#if 0
#include <stdio.h>
#include <stddef.h>
#include <iso646.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
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
#endif

#include <cstdio>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <cinttypes>
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <ctime>
#include <cwchar>
#include <cwctype>

#ifdef getc
//#error defined getc
#endif

static_assert( sizeof(std::int64_t) == 8, "FAILED: sizeof(std::int64_t) == 8");
//static_assert( sizeof(int32_t) == 4 );
#ifdef COMPILING_FOR_ERROR
static_assert(0, "static_assert(0)");
#endif

bool				b1 = false;
bool				b2 = true;
#ifndef __CCWRAP_NO_THREAD_LOCAL
thread_local bool	b3 = false;
#endif

char	ao1 = alignof(bool);
int 	ao2 = alignof(char);
int 	ao3 = alignof(int);
struct StHasOpAmp {
	bool operator&() const { return false; }
	void* amp;
};
int		ao4 = alignof(StHasOpAmp);


#ifndef __CCWRAP_NO_ALIGNAS
alignas(8)	int		aa1 = false;
alignas(8)	int		aa2 = false;
#endif


struct C_Base {
	virtual ~C_Base() {}
	virtual void membFunc() = 0;
	virtual void membFunc2() = 0;
};


struct C1 : public C_Base {
	virtual ~C1() {}
	virtual void membFunc() override { std::printf("C1 override\n"); }
	virtual void membFunc2() final { std::printf("C1 final\n"); }
 #ifdef COMPILING_FOR_ERROR
	virtual void membFunc3() override { std::printf("C1 override xxx\n"); }
 #endif
};
struct C2 : public C1 {
	virtual ~C2() {}
	virtual void membFunc() override { std::printf("C2 override\n"); }
 #ifdef COMPILING_FOR_ERROR
	virtual void membFunc2() final { std::printf("C2 final\n"); }
 #endif
};


int main(int argc, char* argv[])
{
	static_assert(sizeof(C_Base) > 0, "");
 #if 0 //ndef alignof
	static_assert(alignof(C_Base) == sizeof(void*), "");
 #endif
	//static_assert(alignof(C1) >= sizeof(void*), "");	// maybe
	std::printf("\n%s\n", argv[0]);
	std::printf("%s (%d): %s -- __FILE__,__LINE__,__func__\n", __FILE__, __LINE__, __func__);
	std::printf("\t__cplusplus = %d\n", int(__cplusplus));
	std::printf("\tsizeof(bool) = %d\n", int(sizeof(bool)));
	std::printf("\tsizeof(wchar_t) = %d\n", int(sizeof(wchar_t)));
	std::printf("\tsizeof(std::intmax_t) = %2d  _Alignof(std::intmax_t) = %2d\n", int(sizeof(std::intmax_t)), int(alignof(std::intmax_t)));
	std::printf("\tsizeof(long double) = %2d  _Alignof(long double) = %2d\n", int(sizeof(long double)), int(alignof(long double)));
	std::printf("\talignof(std::max_align_t) = %d\n", int(alignof(std::max_align_t)));

	assert(ao1 == 1);
	assert(ao2 == 1);
	assert(ao3 == 4);
	assert(ao4 == sizeof(void*));
	assert(alignof(std::uint64_t) == 8);

	char	buf[1024] = {0};
	char* 	p 		  = new char[10];
	assert(p != nullptr);
	std::strcpy(p, "hello");
	std::snprintf(buf, sizeof buf, "%s world! (%d)", p, int(std::strlen(p)));
	//assert(strcasecmp(p, "HELLO") == 0);
	delete[] p;

 #ifdef PRId64
	std::snprintf(buf, sizeof buf, "# %" PRId64 "  %" PRIx64 "\n", INT64_C(1234567890123456789), INT64_C(0x123456789abcdef0) );
	assert(std::strcmp(buf, "# 1234567890123456789  123456789abcdef0\n") == 0);
 #endif

 #ifndef __CCWRAP_NO_ALIGNAS
	if ((uintptr_t(&aa1) & 7) != 0) std::printf("bad alignas(8) aa1\n");
	if ((uintptr_t(&aa2) & 7) != 0) std::printf("bad alignas(8) aa2\n");
 #endif

	std::printf("\n");

	return 0;
}



#ifdef COMPILING_FOR_ERROR
#error [COMPILE ERROR TEST] done
#endif
