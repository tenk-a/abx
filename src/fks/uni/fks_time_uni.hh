/**
 *  @file   fks_time_uni.hh
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#include <fks/fks_time.h>
#include <unistd.h>
//#include <sys/stat.h>
#include <stdio.h>
#include <time.h>


#define TIME_TO_TIMESPEC_INI(t)		{ ((t) / 1000LL), ((t) % 1000LL) * 1000000ULL }



FKS_LIB_DECL(fks_time_t)
fks_getCurrentGlobalTime() FKS_NOEXCEPT
{
 #if 1
	fks_timespec ts = fks_getCurrentGlobalTimespec();
	return FKS_TIMESPEC_TO_TIME(ts);
 #else
    return time(NULL) * 1000UL;
 #endif
}


FKS_LIB_DECL(fks_timespec)
fks_getCurrentGlobalTimespec() FKS_NOEXCEPT
{
 #if defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L
	fks_timespec ts;
	if  (timespec_get((struct timespec*)&ts, TIME_UTC) == 0)
		ts.tv_sec = ts.tv_nsec = 0;
	return ts;
 #elif defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 199309L
	struct timespec ts = {0,0};
 	clock_gettime(CLOCK_REALTIME, &ts);
 	return *(fks_timespec*)&ts;
 #else
	fks_timespec rt = { time(NULL), 0 };
    return rt;
 #endif
}


FKS_LIB_DECL(fks_timespec)
fks_globalDateTimeToTimespec(Fks_DateTime const* dateTime) FKS_NOEXCEPT
{
    struct tm  		tmWk = {0};
    tmWk.tm_year    = dateTime->year - 1900;
    tmWk.tm_mon     = dateTime->month - 1;
    tmWk.tm_wday    = dateTime->dayOfWeek;
    tmWk.tm_mday    = dateTime->day;
    tmWk.tm_hour    = dateTime->hour;
    tmWk.tm_min     = dateTime->minute;
    tmWk.tm_sec     = dateTime->second;
	{
		fks_timespec	rt;
	    rt.tv_sec 		= timegm(&tmWk);
		rt.tv_nsec 		= dateTime->milliSeconds * 1000 + dateTime->microSeconds + dateTime->nanoSeconds;
	    return rt;
	}
}


FKS_LIB_DECL(fks_timespec)
fks_localDateTimeToTimespec(Fks_DateTime const* dateTime) FKS_NOEXCEPT
{
    struct tm  		tmWk = {0};
    tmWk.tm_year    = dateTime->year - 1900;
    tmWk.tm_mon     = dateTime->month - 1;
    tmWk.tm_wday    = dateTime->dayOfWeek;
    tmWk.tm_mday    = dateTime->day;
    tmWk.tm_hour    = dateTime->hour;
    tmWk.tm_min     = dateTime->minute;
    tmWk.tm_sec     = dateTime->second;
	{
		fks_timespec	rt;
	    rt.tv_sec 		= mktime(&tmWk);
		rt.tv_nsec 		= dateTime->milliSeconds * 1000 + dateTime->microSeconds + dateTime->nanoSeconds;
	    return rt;
	}
}


FKS_LIB_DECL (Fks_DateTime*)
fks_timespecToLocalDateTime(fks_timespec const* ts, Fks_DateTime* dateTime) FKS_NOEXCEPT
{
    time_t      ft   = ts->tv_sec;
    struct tm   tmWk = {0};
    struct tm*  rc   = localtime_r(&ft, &tmWk );
    uint64_t    nsec;
//printf("%d-%d-%d\n", tmWk.tm_year, tmWk.tm_mon + 1, tmWk.tm_mday);
    dateTime->year              = tmWk.tm_year + 1900;
    dateTime->month             = tmWk.tm_mon + 1;
    dateTime->dayOfWeek         = tmWk.tm_wday;
    dateTime->day               = tmWk.tm_mday;
    dateTime->hour              = tmWk.tm_hour;
    dateTime->minute            = tmWk.tm_min;
    dateTime->second            = tmWk.tm_sec;
    nsec = ts->tv_nsec;
    dateTime->milliSeconds      = (nsec / 1000000);
    dateTime->microSeconds      = (nsec / 1000) % 1000;
    dateTime->nanoSeconds       = nsec % 1000;

    return rc ? dateTime : NULL;
}


FKS_LIB_DECL (Fks_DateTime*)
fks_timespecToGlobalDateTime(fks_timespec const* ts, Fks_DateTime* dateTime) FKS_NOEXCEPT
{
    time_t      ft   = ts->tv_sec;
    struct tm   tmWk = {0};
    struct tm*  rc   = gmtime_r(&ft, &tmWk );
    uint64_t    nsec;
//printf("%d-%d-%d\n", tmWk.tm_year, tmWk.tm_mon + 1, tmWk.tm_mday);
    dateTime->year              = tmWk.tm_year + 1900;
    dateTime->month             = tmWk.tm_mon + 1;
    dateTime->dayOfWeek         = tmWk.tm_wday;
    dateTime->day               = tmWk.tm_mday;
    dateTime->hour              = tmWk.tm_hour;
    dateTime->minute            = tmWk.tm_min;
    dateTime->second            = tmWk.tm_sec;
    nsec = ts->tv_nsec;
    dateTime->milliSeconds      = (nsec / 1000000);
    dateTime->microSeconds      = (nsec / 1000) % 1000;
    dateTime->nanoSeconds       = nsec % 1000;

    return rc ? dateTime : NULL;
}


FKS_LIB_DECL(fks_time_t)
fks_globalDateTimeToTime(Fks_DateTime const* dateTime) FKS_NOEXCEPT
{
	fks_timespec ts = fks_globalDateTimeToTimespec(dateTime);
	return FKS_TIMESPEC_TO_TIME(ts);
}


FKS_LIB_DECL(fks_time_t)
fks_localDateTimeToTime(Fks_DateTime const* dateTime) FKS_NOEXCEPT
{
	fks_timespec ts = fks_localDateTimeToTimespec(dateTime);
	return FKS_TIMESPEC_TO_TIME(ts);
}


FKS_LIB_DECL (Fks_DateTime*)
fks_timeToLocalDateTime(fks_time_t fileTime, Fks_DateTime* dateTime) FKS_NOEXCEPT
{
	fks_timespec	ts = TIME_TO_TIMESPEC_INI(fileTime);
	return fks_timespecToLocalDateTime(&ts, dateTime);
}


FKS_LIB_DECL (Fks_DateTime*)
fks_timeToGlobalDateTime(fks_time_t fileTime, Fks_DateTime* dateTime) FKS_NOEXCEPT
{
	fks_timespec	ts = TIME_TO_TIMESPEC_INI(fileTime);
	return fks_timespecToGlobalDateTime(&ts, dateTime);
}
