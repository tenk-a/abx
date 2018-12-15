#include <fks_time.h>
#include <unistd.h>
//#include <sys/stat.h>
#include <stdio.h>
#include <time.h>


FKS_LIB_DECL(fks_time_t)
fks_getCurrentGlobalFileTime() FKS_NOEXCEPT
{
	return time(NULL) * 1000UL;
}



FKS_LIB_DECL(fks_time_t)
fks_globalDateTimeToFileTime(Fks_DateTime const* dateTime) FKS_NOEXCEPT
{
	struct tm  tmWk = {0};
	uint64_t	sec;

    tmWk.tm_year	= dateTime->year - 1900;
    tmWk.tm_mon		= dateTime->month - 1;
    tmWk.tm_wday	= dateTime->dayOfWeek;
    tmWk.tm_mday	= dateTime->day;
    tmWk.tm_hour	= dateTime->hour;
    tmWk.tm_min		= dateTime->minute;
    tmWk.tm_sec		= dateTime->second;
	sec = timegm(&tmWk);
	return sec * 1000 + dateTime->milliSeconds;
}

FKS_LIB_DECL(fks_time_t)
fks_localDateTimeToFileTime(Fks_DateTime const* dateTime) FKS_NOEXCEPT
{
	struct tm  tmWk = {0};
	uint64_t	sec;

    tmWk.tm_year	= dateTime->year - 1900;
    tmWk.tm_mon		= dateTime->month - 1;
    tmWk.tm_wday	= dateTime->dayOfWeek;
    tmWk.tm_mday	= dateTime->day;
    tmWk.tm_hour	= dateTime->hour;
    tmWk.tm_min		= dateTime->minute;
    tmWk.tm_sec		= dateTime->second;
	sec = mktime(&tmWk);
	return sec * 1000 + dateTime->milliSeconds;
}


FKS_LIB_DECL (Fks_DateTime*)
fks_fileTimeToLocalDateTime(fks_time_t fileTime, Fks_DateTime* dateTime) FKS_NOEXCEPT
{
	time_t		ft   = fileTime / 1000;
	struct tm  	tmWk = {0};
	struct tm* 	rc   = localtime_r(&ft, &tmWk );
	uint64_t	nsec;
//printf("%d-%d-%d\n", tmWk.tm_year, tmWk.tm_mon + 1, tmWk.tm_mday);
    dateTime->year				= tmWk.tm_year + 1900;
    dateTime->month				= tmWk.tm_mon + 1;
    dateTime->dayOfWeek			= tmWk.tm_wday;
    dateTime->day				= tmWk.tm_mday;
    dateTime->hour				= tmWk.tm_hour;
    dateTime->minute			= tmWk.tm_min;
    dateTime->second			= tmWk.tm_sec;

    nsec = ((fileTime % 1000) * 10000000) % 1000000000;
    dateTime->milliSeconds		= (nsec / 1000000);
    dateTime->microSeconds		= (nsec / 1000) % 1000;
    dateTime->nanoSeconds		= nsec % 1000;

	return rc ? dateTime : NULL;
}


FKS_LIB_DECL (Fks_DateTime*)
fks_fileTimeToGlobalDateTime(fks_time_t fileTime, Fks_DateTime* dateTime) FKS_NOEXCEPT
{
	time_t		ft   = fileTime / 1000;
	struct tm  	tmWk = {0};
	struct tm* 	rc   = gmtime_r(&ft, &tmWk );
	uint64_t	nsec;

    dateTime->year				= tmWk.tm_year + 1900;
    dateTime->month				= tmWk.tm_mon + 1;
    dateTime->dayOfWeek			= tmWk.tm_wday;
    dateTime->day				= tmWk.tm_mday;
    dateTime->hour				= tmWk.tm_hour;
    dateTime->minute			= tmWk.tm_min;
    dateTime->second			= tmWk.tm_sec;

    nsec = ((fileTime % 1000) * 10000000) % 1000000000;
    dateTime->milliSeconds		= (nsec / 1000000);
    dateTime->microSeconds		= (nsec / 1000) % 1000;
    dateTime->nanoSeconds		= nsec % 1000;

	return rc ? dateTime : NULL;
}
