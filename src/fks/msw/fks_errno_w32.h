/**
 *	@file	errno.h
 *	@brief	error no
 *	@author	tenk* (Masashi Kitamura)
 *	@date	2000-2011
 *	@license	Boost Software Lisence Version 1.0
 *	@note
 *		win32 error-code-bit
 *        3                   2                   1                   0
 *      1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *       --- - - ---------------------- -------------------------------
 *		|Sev|C|R|    facility code     |             code              |
 *      | 3 |1|1|         12           |              16               |
 *       --- - - ---------------------- -------------------------------
 *		Sev - serverity code  00:Success 01:Information 10:Warning 11:Error
 *		C	- Curstomer code flag
 *		R	- Reserve
 */

#ifndef FKS_WIN_ERRNO_H
#define FKS_WIN_ERRNO_H

#include <fks_config.h>

#ifdef FKS_HAS_PRAGMA_ONCE
#pragma once
#endif

#define FKS_EDOM			1212/*ERROR_INVALID_DOMAINNAME*/	/* 33 Domain error (math functions) */
#define	FKS_ERANGE			13/*ERROR_INVALID_DATA*/			/* 34 Result too large (possibly too small) */
#define	FKS_EILSEQ			1113/*ERROR_NO_UNICODE_TRANSLATION*/ /* 42 Illegal byte sequence */

#if 1 // ms-c
/* The value only is set, and not confirmed suitably. */
#define FKS_EPERM			1/*ERROR_INVALID_FUNCTION*/			/*	1 Operation not permitted */
#define	FKS_ENOFILE			2/*ERROR_FILE_NOT_FOUND*/			/*	2 No such file or directory */
#define	FKS_ENOENT			3/*ERROR_PATH_NOT_FOUND*/
#define	FKS_ESRCH			1067/*ERROR_PROCESS_ABORTED*/		/*	3 No such process */
#define	FKS_EINTR			1359/*ERROR_INTERNAL_ERROR*/		/*	4 Interrupted function call */
#define	FKS_EIO				5/*ERROR_ACCESS_DENIED*/			/*	5 Input/output error */
#define	FKS_ENXIO			1117/*ERROR_IO_DEVICE*/				/*	6 No such device or address */
#define	FKS_E2BIG			160/*ERROR_BAD_ARGUMENTS*/			/*	7 Arg list too long */
#define	FKS_ENOEXEC			191/*ERROR_INVALID_EXE_SIGNATURE*/	/*  8 Exec format error */
#define	FKS_EBADF			6/*ERROR_INVALID_HANDLE*/			/*	9 Bad file descriptor */
#define FKS_ECHILD			89/*ERROR_NO_PROC_SLOTS*/			/* 10 No child processes */
//#define FKS_EAGAIN		1450/*ERROR_NO_SYSTEM_RESOURCES*/	/* 11 Resource temporarily unavailable */
#define	FKS_ENOMEM			8/*ERROR_NOT_ENOUGH_MEMORY*/		/* 12 Not enough space */
#define	FKS_EACCES			5/*ERROR_ACCESS_DENIED*/			/* 13 Permission denied */
#define	FKS_EFAULT			487/*ERROR_INVALID_ADDRESS*/		/* 14 Bad address */
																/* 15 - Unknown Error */
#define	FKS_EBUSY			142/*ERROR_BUSY_DRIVE*/				/* 16 strerror reports "Resource device" */
#define	FKS_EEXIST			80/*ERROR_FILE_EXISTS*/				/* 17 File exists */
//#define FKS_EXDEV			1142/*ERROR_TOO_MANY_LINKS*/		/* 18 Improper link (cross-device link?) */
#define FKS_ENODEV			55/*ERROR_DEV_NOT_EXIST*/			/* 19 No such device */
#define FKS_ENOTDIR			267/*ERROR_DIRECTORY*/				/* 20 Not a directory */
#define FKS_EISDIR			0x08000015							/* 21 Is a directory */
#define	FKS_EINVAL			160/*ERROR_BAD_ARGUMENTS*/			/* 22 Invalid argument */
#define	FKS_ENFILE			4/*ERROR_TOO_MANY_OPEN_FILES*/		/* 23 Too many open files in system */
#define	FKS_EMFILE			4/*ERROR_TOO_MANY_OPEN_FILES*/		/* 24 Too many open files */
#define FKS_ENOTTY 			20/*ERROR_BAD_UNIT*/				/* 25 Inappropriate I/O control operation */
																/* 26 - Unknown Error */
//#define FKS_EFBIG			0x80030111/*STG_E_DOCFILETOOLARGE*/	/* 27 File too large */
#define FKS_EFBIG			0x0800001b							/* 27 File too large */
#define	FKS_ENOSPC			112/*ERROR_DISK_FULL*/				/* 28 No space left on device */
#define	FKS_ESPIPE			25/*ERROR_SEEK*/					/* 29 Invalid seek (seek on a pipe?) */
#define	FKS_EROFS			1013/*ERROR_CANTWRITE*/				/* 30 Read-only file system */
#define	FKS_EMLINK			1142/*ERROR_TOO_MANY_LINKS*/		/* 31 Too many links */
#define	FKS_EPIPE			230/*ERROR_BAD_PIPE*/				/* 32 Broken pipe */
//#define FKS_EDOM			1212/*ERROR_INVALID_DOMAINNAME*/	/* 33 Domain error (math functions) */
																/* 35 - Unknown Error */
#define	FKS_EDEADLOCK		212/*ERROR_LOCKED*/					/* 36 Resource deadlock avoided (non-Cyg) */
#define	FKS_EDEADLK			212/*ERROR_LOCKED*/
																/* 37 - Unknown Error */
#define	FKS_ENAMETOOLONG	161/*ERROR_BAD_PATHNAME*/			/* 38 Filename too long (91 in Cyg?) */
//#define	FKS_ENOLCK											/* 39 No locks available (46 in Cyg?) */
//#define	FKS_ENOSYS											/* 40 Function not implemented (88 in Cyg?) */
#define	FKS_ENOTEMPTY		145/*ERROR_DIR_NOT_EMPTY*/			/* 41 Directory not empty (90 in Cyg?) */
#endif

#ifdef __cplusplus
extern "C" {
#endif
FKS_LIB_DECL (int)			fks_get_errno(void);
FKS_LIB_DECL (int)			fks_set_errno(int no);
#define FKS_GET_ERRNO()		fks_get_errno()
#define FKS_SET_ERRNO(no)	fks_set_errno(no)
FKS_EXTERN_C_END
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

struct fks_errno_cpp_t {
public:
	operator  int() const { return fks_get_errno(); }
	int operator=(int no) { return fks_set_errno(no); }
};
fks_errno_cpp_t	fks_errno;

#endif	// __cplusplus

#endif	// _ERRNO_H_
