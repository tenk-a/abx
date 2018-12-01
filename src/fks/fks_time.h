/*
 *  @file   fks_time.h
 *  @brief  System level I/O functions and types.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_TIME_H_INCLUDED
#define FKS_TIME_H_INCLUDED

#include <fks_common.h>
#include <fks_types.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Fks_DateTime {
    int32_t     year;
    uint8_t     month;
    uint8_t     dayOfWeek;
    uint8_t     day;
    uint8_t     hour;
    uint8_t     minute;
    uint8_t     second;
    uint16_t    milliSeconds;
    uint16_t    microSeconds;
    uint16_t    nanoSeconds;
} Fks_DateTime;


FKS_LIB_DECL(Fks_DateTime*)     fks_getCurrentGlobalDateTime(Fks_DateTime* globalDateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(Fks_DateTime*)     fks_getCurrentLocalDateTime(Fks_DateTime* localDateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_time_t)        fks_getCurrentGlobalFileTime() FKS_NOEXCEPT;
FKS_LIB_DECL(fks_time_t)        fks_getCurrentLocalFileTime() FKS_NOEXCEPT;
FKS_LIB_DECL(Fks_DateTime const*) fks_setCurrentGlobalDateTime(Fks_DateTime const* globalDateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(Fks_DateTime const*) fks_setCurrentLocalDateTime(Fks_DateTime const* localDateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_time_t)        fks_setCurrentGlobalFileTime(fks_time_t globalTime) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_time_t)        fks_setCurrentLocalFileTime(fks_time_t localTime) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_time_t)        fks_fileTimeGlobalToLocal(fks_time_t globalFileTime) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_time_t)        fks_fileTimeLocalToGlobal(fks_time_t localFileTime) FKS_NOEXCEPT;
FKS_LIB_DECL(Fks_DateTime*)     fks_fileTimeToLocalDateTime(fks_time_t fileTime, Fks_DateTime* dateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(Fks_DateTime*)     fks_fileTimeToDateTime(fks_time_t fileTime, Fks_DateTime* dateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_time_t)        fks_localDateTimeToFileTime(Fks_DateTime const* dateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_time_t)        fks_dateTimeToFileTime(Fks_DateTime const* dateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(Fks_DateTime*)     fks_dateTimeGlobalToLocal(Fks_DateTime const* globalTime, Fks_DateTime* localTime) FKS_NOEXCEPT;
FKS_LIB_DECL(Fks_DateTime*)     fks_dateTimeLocalToGlobal(Fks_DateTime const* localTime, Fks_DateTime* globalTime) FKS_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif	// FKS_TIME_H_INCLUDED
