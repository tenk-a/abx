/**
 *  @file  fks_fks_perfcnt.h
 *  @brief paformance counter
 *  @author tenk* (Masashi Kitamura)
 *  @license Boost Software Lisence Version 1.0
 */
#include <fks/fks_perfcnt.h>

#ifdef FKS_WIN32
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL(fks_perfcnt_t)  fks_perfcnt_get(void)
{
    fks_perfcnt_t c;
    QueryPerformanceCounter((LARGE_INTEGER*)&c);
    return c;
}

FKS_LIB_DECL(fks_perfcnt_t)  fks_perfcnt_per_sec(void)
{
    static fks_perfcnt_t s = 0;
    if (!s)
        QueryPerformanceFrequency((LARGE_INTEGER*)&s);
    return s;
}

#ifdef __cplusplus
}
#endif

#elif 1 //defined FKS_LINUX

#include <sys/resource.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL(fks_perfcnt_t)  fks_perfcnt_get(void)
{
 #if defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 199309L
    struct timespec ts = {0,0};
    //clock_gettime(CLOCK_REALTIME, &ts);
    clock_gettime(CLOCK_MONOTONIC, &ts);    // CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID
    return ts.tv_sec * 100000000000LL + ts.tv_nsec;
 #elif 1
    struct timeval  tv = {0,0};
    gettimeofday(&tv, NULL);
    return ts.tv_sec * 1000000LL + tv.tv_usec;
 #elif 0
    struct rusage t;
    getrusage(RUSAGE_SELF, &t);
    return t.ru_utime.tv_sec * 1000ULL + t.ru_utime.tv_usec;
    //return t.ru_stime.tv_sec * 1000ULL + t.ru_stime.tv_usec;
 #else
    return clock();
 #endif
}

FKS_LIB_DECL(fks_perfcnt_t)  fks_perfcnt_per_sec(void)
{
 #if defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 199309L
    return 1000000000LL;
 #elif 1
    return 1000000ULL;
 #elif 0
    return 1000000ULL;
 #else
    return CLOCKS_PER_SEC;
 #endif
}

#ifdef __cplusplus
}
#endif

#endif
