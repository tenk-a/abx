/**
 *  @file  fks_perfcnt.h
 *  @brief paformance counter
 *  @author tenk* (Masashi Kitamura)
 *  @license Boost Software Lisence Version 1.0
 */

#ifndef FKS_PERFCNT_H_INCLUDED
#define FKS_PERFCNT_H_INCLUDED

#include <fks_common.h>
#include <stdint.h>

#ifdef FKS_WIN32
  typedef uint64_t  fks_perfcnt_t;
#elif defined FKS_LINUX
  typedef uint64_t  fks_perfcnt_t;
  #define           fks_perfcnt_per_sec()       1000000ULL
#else
  #include <time.h>
  typedef clock_t   fks_perfcnt_t;
  #define           fks_perfcnt_get()           clock()
  #define           fks_perfcnt_per_sec()       CLOCKS_PER_SEC
#endif

#ifdef __cplusplus
extern "C" {
#endif
fks_perfcnt_t  fks_perfcnt_get(void);
fks_perfcnt_t  fks_perfcnt_per_sec(void);

#define FKS_PERFCNT_TO_SEC(t)      	((t) * 1.0 / fks_perfcnt_per_sec())
#define FKS_PERFCNT_TO_MILLI_SEC(t) ((t) * (fks_perfcnt_t)(1000) / fks_perfcnt_per_sec())
#define FKS_PERFCNT_TO_MICRO_SEC(t) ((t) * (fks_perfcnt_t)(1000000) / fks_perfcnt_per_sec())

#ifdef __cplusplus
}
#endif

#endif	// FKS_PERFCNT_H_INCLUDED
