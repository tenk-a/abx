/**
 *  @file  fks_fks_perfcnt.h
 *  @brief paformance counter
 *  @author tenk* (Masashi Kitamura)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks_perfcnt.h>

#ifdef FKS_WIN32
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL(fks_perfcnt_t)  fks_perfcnt_get(void) FKS_NOEXCEPT
{
	fks_perfcnt_t c;
	QueryPerformanceCounter((LARGE_INTEGER*)&c);
	return c;
}

FKS_LIB_DECL(fks_perfcnt_t)  fks_perfcnt_per_sec(void) FKS_NOEXCEPT
{
	static fks_perfcnt_t s = 0;
	if (!s)
		QueryPerformanceFrequency((LARGE_INTEGER*)&s);
	return s;
}

#ifdef __cplusplus
}
#endif

#elif defined FKS_LINUX
#include <sys/resource.h>

#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL(fks_perfcnt_t)  fks_perfcnt_get(void) FKS_NOEXCEPT
{
	struct rusage t;
	getrusage(RUSAGE_SELF, &t);
	return t.ru_utime.tv_sec * 1000ULL + t.ru_utime.tv_usec;
	//return t.ru_stime.tv_sec * 1000ULL + t.ru_stime.tv_usec;
}

#ifdef __cplusplus
}
#endif

#endif
