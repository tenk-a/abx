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

//#if !defined FKS_WIN32 && !defined FKS_LINUX
// #include <time.h>
//#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FKS_WIN32
  typedef uint64_t  fks_perfcnt_t;
#elif 1 //defined FKS_LINUX
  typedef uint64_t  fks_perfcnt_t;
#endif

FKS_LIB_DECL(fks_perfcnt_t)  	fks_perfcnt_get(void) FKS_NOEXCEPT;
  FKS_LIB_DECL(fks_perfcnt_t)   fks_perfcnt_per_sec(void) FKS_NOEXCEPT;

#define FKS_PERFCNT_TO_SEC(t)       ((t) * 1.0 / fks_perfcnt_per_sec())
#define FKS_PERFCNT_TO_MILLI_SEC(t) ((t) * (fks_perfcnt_t)(1000) / fks_perfcnt_per_sec())
#define FKS_PERFCNT_TO_MICRO_SEC(t) ((t) * (fks_perfcnt_t)(1000000) / fks_perfcnt_per_sec())

#ifdef __cplusplus
}
#endif

#endif  // FKS_PERFCNT_H_INCLUDED
