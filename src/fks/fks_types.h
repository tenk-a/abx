/*
 *  @file   fks_types.h
 *  @brief  System level I/O functions and types.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_TYPES_H_INCLUDED
#define FKS_TYPES_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

typedef int64_t         fks_off_t;
typedef int64_t         fks_off64_t;
typedef int64_t         fks_isize_t;
#define FKS_ISIZE_MIN   (-FKS_LLONG_C(9223372036854775807) - 1)
#define FKS_ISIZE_MAX   (FKS_LLONG_C(9223372036854775807))

#if 1
typedef int64_t         fks_time_t;         // milli sec.
#define FKS_TIME_MIN    (-FKS_LLONG_C(9223372036854775807) - 1)
#define FKS_TIME_MAX    (FKS_LLONG_C(9223372036854775807))
#else
typedef uint64_t        fks_time_t;         // milli sec.
#define FKS_TIME_MIN    0
#define FKS_TIME_MAX    (FKS_ULLONG_C(0xFFFFffffFFFFffff))
#endif

typedef struct fks_ftime_t {
	uint64_t	sec;
	uint64_t	nsec;
} fks_ftime_t;

#endif
