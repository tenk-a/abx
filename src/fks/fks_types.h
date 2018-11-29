/*
 *  @file   fks_types.h
 *  @brief  System level I/O functions and types.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_TYPES_H_INCLUDED
#define FKS_TYPES_H_INCLUDED

#include <stdint.h>

typedef int64_t         fks_time_t;         // nano sec.
typedef int64_t         fks_off_t;
typedef int64_t         fks_isize_t;
typedef int64_t         fks_off64_t;

#define FKS_TIME_MIN    (-FKS_LLONG_C(9223372036854775807) - 1)
#define FKS_TIME_MAX    (FKS_LLONG_C(9223372036854775807))
#define FKS_ISIZE_MIN   (-FKS_LLONG_C(9223372036854775807) - 1)
#define FKS_ISIZE_MAX   (FKS_LLONG_C(9223372036854775807))

#endif
