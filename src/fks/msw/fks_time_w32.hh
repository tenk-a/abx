/*
 *  @file   fks_time_w32.hh
 *  @brief  System level I/O functions and types.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#include <fks_time.h>
#include <fks_assert_ex.h>
#include <windows.h>


#define TIME_TO_TIMESPEC_INI(t)		{ ((t) / 1000), ((t) % 1000) * 1000000 }


#ifdef __cplusplus
extern "C" {
#endif

FKS_STATIC_DECL(Fks_DateTime*)  fks_w32systemTimeToFksDateTime(SYSTEMTIME const* src, Fks_DateTime* dst) FKS_NOEXCEPT;
FKS_STATIC_DECL(SYSTEMTIME*)    fks_fksDateTimeToW32SystemTime(Fks_DateTime const* src, SYSTEMTIME* dst) FKS_NOEXCEPT;

FKS_LIB_DECL(Fks_DateTime*) fks_getCurrentGlobalDateTime(Fks_DateTime* globalDateTime) FKS_NOEXCEPT
{
    SYSTEMTIME st = {0};
    GetSystemTime(&st);
    return fks_w32systemTimeToFksDateTime(&st, globalDateTime);
}

FKS_LIB_DECL(Fks_DateTime*) fks_getCurrentLocalDateTime(Fks_DateTime* localDateTime) FKS_NOEXCEPT
{
    SYSTEMTIME st = {0};
    GetLocalTime(&st);
    return fks_w32systemTimeToFksDateTime(&st, localDateTime);
}

FKS_LIB_DECL(fks_time_t) fks_getCurrentGlobalTime() FKS_NOEXCEPT
{
    SYSTEMTIME  st = {0};
    fks_time_t  tm = 0;
    GetSystemTime(&st);
    if (SystemTimeToFileTime(&st, (FILETIME*)&tm) == 0) {
        return 0;
    }
    return tm;
}

FKS_LIB_DECL(fks_time_t) fks_getCurrentLocalTime() FKS_NOEXCEPT
{
    SYSTEMTIME  st = {0};
    fks_time_t  tm = 0;
    GetLocalTime(&st);
    if (SystemTimeToFileTime(&st, (FILETIME*)&tm) == 0) {
        return 0;
    }
    return tm;
}

FKS_LIB_DECL(Fks_DateTime const*) fks_setCurrentGlobalDateTime(Fks_DateTime const* globalDateTime) FKS_NOEXCEPT
{
    SYSTEMTIME  st = {0};
    fks_fksDateTimeToW32SystemTime(globalDateTime, &st);
    return SetSystemTime(&st) ? globalDateTime : NULL;
}

FKS_LIB_DECL(Fks_DateTime const*) fks_setCurrentLocalDateTime(Fks_DateTime const* localDateTime) FKS_NOEXCEPT
{
    SYSTEMTIME  st = {0};
    fks_fksDateTimeToW32SystemTime(localDateTime, &st);
    return SetLocalTime(&st) ? localDateTime : NULL;
}

FKS_LIB_DECL(int) fks_setCurrentGlobalTime(fks_time_t globalTime) FKS_NOEXCEPT
{
    SYSTEMTIME  st = {0};
    if (FileTimeToSystemTime((FILETIME const*)&globalTime, &st) == 0)
        return 0;
    return SetSystemTime(&st) ? 1 : 0;
}

FKS_LIB_DECL(int) fks_setCurrentLocalTime(fks_time_t localTime) FKS_NOEXCEPT
{
    SYSTEMTIME  st = {0};
    if (FileTimeToSystemTime((FILETIME const*)&localTime, &st) == 0)
        return 0;
    return SetLocalTime(&st) ? 1 : 0;
}

FKS_LIB_DECL(fks_time_t) fks_timeGlobalToLocal(fks_time_t globalFileTime) FKS_NOEXCEPT
{
    fks_time_t  localFT = 0;
    if (FileTimeToLocalFileTime((FILETIME const*)&globalFileTime, (FILETIME*)&localFT) == 0)
        return 0;
    return localFT;
}

FKS_LIB_DECL(fks_time_t) fks_timeLocalToGlobal(fks_time_t localFileTime) FKS_NOEXCEPT
{
    fks_time_t  globalFT = 0;
    if (LocalFileTimeToFileTime((FILETIME const*)&localFileTime, (FILETIME*)&globalFT) == 0)
        return 0;
    return globalFT;
}


FKS_LIB_DECL(Fks_DateTime*) fks_timeToLocalDateTime(fks_time_t fileTime, Fks_DateTime* dateTime) FKS_NOEXCEPT
{
    SYSTEMTIME  st = {0};
    fks_time_t  localFT = 0;
    FKS_ARG_PTR_ASSERT(2, dateTime);
    if (!dateTime)
        return NULL;
    if (FileTimeToLocalFileTime((FILETIME const*)&fileTime, (FILETIME*)&localFT) == 0) {
        memset(dateTime,0, sizeof *dateTime);
        return NULL;
    }
    if (FileTimeToSystemTime((FILETIME const*)&localFT, &st) == 0) {
        memset(dateTime,0, sizeof *dateTime);
        return NULL;
    }
    return fks_w32systemTimeToFksDateTime(&st, dateTime);
}

FKS_LIB_DECL(Fks_DateTime*) fks_timeToGlobalDateTime(fks_time_t fileTime, Fks_DateTime* dateTime) FKS_NOEXCEPT
{
    SYSTEMTIME  st = {0};
    FKS_ARG_PTR_ASSERT(2, dateTime);
    if (!dateTime)
        return NULL;
    if (FileTimeToSystemTime((FILETIME const*)&fileTime, &st) == 0) {
        memset(dateTime,0, sizeof *dateTime);
        return NULL;
    }
    return fks_w32systemTimeToFksDateTime(&st, dateTime);
}

FKS_STATIC_DECL(Fks_DateTime*) fks_w32systemTimeToFksDateTime(SYSTEMTIME const* src, Fks_DateTime* dst) FKS_NOEXCEPT
{
    FKS_ARG_PTR_ASSERT(1, src);
    FKS_ARG_PTR_ASSERT(2, dst);
    dst->year           = src->wYear;
    dst->month          = (uint8_t)src->wMonth;
    dst->dayOfWeek      = (uint8_t)src->wDayOfWeek;
    dst->day            = (uint8_t)src->wDay;
    dst->hour           = (uint8_t)src->wHour;
    dst->minute         = (uint8_t)src->wMinute;
    dst->second         = (uint8_t)src->wSecond;
    dst->milliSeconds   = src->wMilliseconds;
    dst->microSeconds   = 0;
    dst->nanoSeconds    = 0;
    return dst;
};


FKS_LIB_DECL(fks_time_t)    fks_localDateTimeToTime(Fks_DateTime const* dateTime) FKS_NOEXCEPT
{
    fks_time_t  tm = 0;
    SYSTEMTIME  sysTm;
    FKS_ARG_PTR_ASSERT(1, dateTime);
    if (!dateTime)
        return 0;
    fks_fksDateTimeToW32SystemTime(dateTime, &sysTm);
    if (SystemTimeToFileTime(&sysTm, (FILETIME*)&tm) == 0)
        return 0;
    return fks_timeLocalToGlobal(tm);
}

FKS_LIB_DECL(fks_time_t)    fks_globalDateTimeToTime(Fks_DateTime const* dateTime) FKS_NOEXCEPT
{
    fks_time_t  tm = 0;
    SYSTEMTIME  sysTm;
    FKS_ARG_PTR_ASSERT(1, dateTime);
    if (!dateTime)
        return 0;
    fks_fksDateTimeToW32SystemTime(dateTime, &sysTm);
    if (SystemTimeToFileTime(&sysTm, (FILETIME*)&tm) == 0) {
        return 0;
    }
    return tm;
}

FKS_STATIC_DECL(SYSTEMTIME*) fks_fksDateTimeToW32SystemTime(Fks_DateTime const* src, SYSTEMTIME* dst) FKS_NOEXCEPT
{
    FKS_ARG_PTR_ASSERT(1, src);
    FKS_ARG_PTR_ASSERT(2, dst);
    dst->wYear          = (WORD)src->year;
    dst->wMonth         = src->month;
    dst->wDayOfWeek     = src->dayOfWeek;
    dst->wDay           = src->day;
    dst->wHour          = src->hour;
    dst->wMinute        = src->minute;
    dst->wSecond        = src->second;
    dst->wMilliseconds  = src->milliSeconds;
    return dst;
};


FKS_LIB_DECL(Fks_DateTime*) fks_dateTimeGlobalToLocal(Fks_DateTime const* globalTime, Fks_DateTime* localTime) FKS_NOEXCEPT
{
    fks_time_t gt = fks_globalDateTimeToTime(globalTime);
    fks_time_t lt = gt ? fks_timeGlobalToLocal(gt) : 0;
    return lt ? fks_timeToGlobalDateTime(lt, localTime) : NULL;
}

FKS_LIB_DECL(Fks_DateTime*) fks_dateTimeLocalToGlobal(Fks_DateTime const* localTime, Fks_DateTime* globalTime) FKS_NOEXCEPT
{
    fks_time_t lt = fks_globalDateTimeToTime(localTime);
    fks_time_t gt = lt ? fks_timeLocalToGlobal(lt) : 0;
    return gt ? fks_timeToGlobalDateTime(gt, globalTime) : NULL;
}

// ------------------------------
// timespec version

FKS_LIB_DECL(fks_timespec) fks_getCurrentGlobalTimespec() FKS_NOEXCEPT
{
	fks_time_t 		t  = fks_getCurrentGlobalTime();
    fks_timespec	rt = TIME_TO_TIMESPEC_INI(t);
    return rt;
}

FKS_LIB_DECL(fks_timespec) fks_getCurrentLocalTimespec() FKS_NOEXCEPT
{
	fks_time_t 		t  = fks_getCurrentLocalTime();
    fks_timespec	rt = TIME_TO_TIMESPEC_INI(t);
    return rt;
}


FKS_LIB_DECL(int) fks_setCurrentGlobalTimespec(fks_timespec const* ts) FKS_NOEXCEPT
{
	return fks_setCurrentGlobalTime(FKS_TIMESPEC_TO_TIME(*ts));
}

FKS_LIB_DECL(int) fks_setCurrentLocalTimespec(fks_timespec const* ts) FKS_NOEXCEPT
{
	return fks_setCurrentLocalTime(FKS_TIMESPEC_TO_TIME(*ts));
}

FKS_LIB_DECL(fks_timespec) fks_timespecGlobalToLocal(fks_timespec const* ts) FKS_NOEXCEPT
{
	fks_time_t		t  = fks_timeGlobalToLocal(FKS_TIMESPEC_TO_TIME(*ts));
    fks_timespec	rt = TIME_TO_TIMESPEC_INI(t);
    return rt;
}

FKS_LIB_DECL(fks_timespec) fks_timespecLocalToGlobal(fks_timespec const* ts) FKS_NOEXCEPT
{
	fks_time_t		t  = fks_timeLocalToGlobal(FKS_TIMESPEC_TO_TIME(*ts));
    fks_timespec	rt = TIME_TO_TIMESPEC_INI(t);
    return rt;
}

FKS_LIB_DECL(Fks_DateTime*) fks_timespecToLocalDateTime(fks_timespec const* ts, Fks_DateTime* dateTime) FKS_NOEXCEPT
{
	return fks_timeToLocalDateTime(FKS_TIMESPEC_TO_TIME(*ts), dateTime);
}

FKS_LIB_DECL(Fks_DateTime*) fks_timespecToGlobalDateTime(fks_timespec const* ts, Fks_DateTime* dateTime) FKS_NOEXCEPT
{
	return fks_timeToGlobalDateTime(FKS_TIMESPEC_TO_TIME(*ts), dateTime);
}

FKS_LIB_DECL(fks_timespec)    fks_localDateTimeToTimespec(Fks_DateTime const* dateTime) FKS_NOEXCEPT
{
	fks_time_t		t = fks_localDateTimeToTime(dateTime);
    fks_timespec	rt = TIME_TO_TIMESPEC_INI(t);
    return rt;
}

FKS_LIB_DECL(fks_timespec)    fks_globalDateTimeToTimespec(Fks_DateTime const* dateTime) FKS_NOEXCEPT
{
	fks_time_t		t = fks_globalDateTimeToTime(dateTime);
    fks_timespec	rt = TIME_TO_TIMESPEC_INI(t);
    return rt;
}

#ifdef __cplusplus
}
#endif
