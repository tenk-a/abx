/*
 *  @file   fks_time.h
 *  @brief  System level I/O functions and types.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_TIME_H_INCLUDED
#define FKS_TIME_H_INCLUDED

#include <fks/fks_common.h>
#include <fks/fks_types.h>
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


FKS_LIB_DECL(fks_time_t)        fks_getCurrentGlobalTime() FKS_NOEXCEPT;
#ifdef FKS_WIN32
FKS_LIB_DECL(fks_time_t)        fks_getCurrentLocalTime() FKS_NOEXCEPT;
#endif
FKS_LIB_DECL(Fks_DateTime*)     fks_timeToLocalDateTime(fks_time_t ftime, Fks_DateTime* dateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(Fks_DateTime*)     fks_timeToGlobalDateTime(fks_time_t ftime, Fks_DateTime* dateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_time_t)        fks_localDateTimeToTime(Fks_DateTime const* dateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_time_t)        fks_globalDateTimeToTime(Fks_DateTime const* dateTime) FKS_NOEXCEPT;


#ifdef FKS_WIN32
FKS_LIB_DECL(int)               fks_setCurrentGlobalTime(fks_time_t globalTime) FKS_NOEXCEPT;
FKS_LIB_DECL(int)               fks_setCurrentLocalTime(fks_time_t localTime) FKS_NOEXCEPT;

FKS_LIB_DECL(fks_time_t)        fks_timeGlobalToLocal(fks_time_t globalFileTime) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_time_t)        fks_timeLocalToGlobal(fks_time_t localFileTime) FKS_NOEXCEPT;
FKS_LIB_DECL(Fks_DateTime*)     fks_dateTimeGlobalToLocal(Fks_DateTime const* globalTime, Fks_DateTime* localTime) FKS_NOEXCEPT;
FKS_LIB_DECL(Fks_DateTime*)     fks_dateTimeLocalToGlobal(Fks_DateTime const* localTime, Fks_DateTime* globalTime) FKS_NOEXCEPT;

FKS_LIB_DECL(Fks_DateTime*)     fks_getCurrentGlobalDateTime(Fks_DateTime* globalDateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(Fks_DateTime*)     fks_getCurrentLocalDateTime(Fks_DateTime* localDateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(Fks_DateTime const*) fks_setCurrentGlobalDateTime(Fks_DateTime const* globalDateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(Fks_DateTime const*) fks_setCurrentLocalDateTime(Fks_DateTime const* localDateTime) FKS_NOEXCEPT;
#endif


FKS_LIB_DECL(fks_timespec)      fks_getCurrentGlobalTimespec() FKS_NOEXCEPT;
#ifdef FKS_WIN32
FKS_LIB_DECL(fks_timespec)      fks_getCurrentLocalTimespec() FKS_NOEXCEPT;
#endif
FKS_LIB_DECL(Fks_DateTime*)     fks_timespecToLocalDateTime(fks_timespec const* ftime, Fks_DateTime* dateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(Fks_DateTime*)     fks_timespecToGlobalDateTime(fks_timespec const* ftime, Fks_DateTime* dateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_timespec)      fks_localDateTimeToTimespec(Fks_DateTime const* dateTime) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_timespec)      fks_globalDateTimeToTimespec(Fks_DateTime const* dateTime) FKS_NOEXCEPT;

#ifdef FKS_WIN32
FKS_LIB_DECL(int)               fks_setCurrentGlobalTimespec(fks_timespec const* globalTime) FKS_NOEXCEPT;
FKS_LIB_DECL(int)               fks_setCurrentLocalTimespec(fks_timespec const* localTime) FKS_NOEXCEPT;

FKS_LIB_DECL(fks_timespec)      fks_timespecGlobalToLocal(fks_timespec const* globalFileTime) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_timespec)      fks_timespecLocalToGlobal(fks_timespec const* localFileTime) FKS_NOEXCEPT;
#endif

FKS_INL_LIB_DECL(int64_t)       fks_timespecCmp(fks_timespec const* l, fks_timespec const* r) FKS_NOEXCEPT {
                                    int64_t  rc = l->tv_sec - r->tv_sec;
                                    if (rc)
                                        return rc;
                                    return l->tv_nsec - r->tv_nsec;
                                }

#ifdef __cplusplus
}
#endif

#endif  // FKS_TIME_H_INCLUDED
