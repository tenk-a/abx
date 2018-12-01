/**
 *  @file  fks_fks_perfcnt.h
 *  @brief paformance counter
 *  @author tenk* (Masashi Kitamura)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks_perfcnt.h>

#ifdef _WIN32
#include <windows.h>

fks_perfcnt_t  fks_perfcnt_get(void)
{
	fks_perfcnt_t c;
	QueryPerformanceCounter((LARGE_INTEGER*)&c);
	return c;
}

fks_perfcnt_t  fks_perfcnt_per_sec(void)
{
	static fks_perfcnt_t s = 0;
	if (!s)
		QueryPerformanceFrequency((LARGE_INTEGER*)&s);
	return s;
}

#elif defined LINUX
#include <sys/resource.h>

fks_perfcnt_t  fks_perfcnt_get(void)
{
	struct rusage t;
	getrusage(RUSAGE_SELF, &t);
	return t.ru_utime.tv_sec * 1000000ULL + t.ru_utime.tv_usec;
}

#endif
