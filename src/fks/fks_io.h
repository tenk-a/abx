/*
 *  @file   fks_io.h
 *  @brief  System level I/O functions and types.
 */
#ifndef FKS_IO_H
#define FKS_IO_H

#include <fks/fks_config.h>
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

// types.h
#ifdef FKS_WIN32
typedef intptr_t		fks_fh_t;
#else
typedef int				fks_fh_t;
#endif

typedef int				fks_io_rc_t;		// 0:OK -:error

typedef int64_t			fks_time_t;			// nano sec.
typedef int64_t		 	fks_off_t;
typedef int64_t		 	fks_fpos64_t;
typedef int64_t			fks_off64_t;
typedef unsigned short  fks_mode_t;

#ifndef FKS_WIN32
typedef short 			fks_ino_t;
typedef unsigned		fks_dev_t;
//typedef int			fks_pid_t;
//typedef int			fks_sigset_t;
//typedef long 			fks_ssize_t;
typedef unsigned int	fks_useconds_t;
#endif


// fcntl.h
enum {
 #ifdef FKS_WIN32
	FKS_O_RDONLY		= 0x00,
	FKS_O_WRONLY		= 0x01,
	FKS_O_RDWR 			= 0x02,
	FKS_O_ACCMODE		= 0x03, //(FKS_O_RDONLY|FKS_O_WRONLY|FKS_O_RDWR),
	FKS_O_APPEND		= 0x0008,
	FKS_O_CREAT			= 0x0100,
	FKS_O_TRUNC			= 0x0200,
	FKS_O_EXCL 			= 0x0400,

	FKS_O_RANDOM		= 0x0010,
	FKS_O_SEQUENTIAL	= 0x0020,
	FKS_O_TEMPORARY		= 0x0040,
	FKS_O_NOINHERIT		= 0x0080,
	FKS_O_SHORT_LIVED	= 0x1000,

	FKS_O_TEXT 			= 0x4000,	// fks-lib not support
	FKS_O_BINARY		= 0x8000,
	FKS_O_RAW			= FKS_O_BINARY,
 #else
	FKS_O_RDONLY 		= 0x00,
	FKS_O_WRONLY 		= 0x01,
	FKS_O_RDWR 			= 0x02,
	FKS_O_ACCMODE 		= 0x03,
	FKS_O_CREAT 		= 0x040,
	FKS_O_EXCL 			= 0x080,
	FKS_O_TRUNC 		= 0x200,
	FKS_O_APPEND 		= 0x400,

	FKS_O_NOCTTY 		= 0x100,
	FKS_O_NONBLOCK 		= 0x800,
	FKS_O_DSYNC 		= 0x1000,
	FKS_O_ASYNC 		= 0x2000,
	FKS_O_DIRECT 		= 0x4000,
	FKS_O_LARGEFILE 	= 0x8000,
	FKS_O_DIRECTORY 	= 0x10000,
	FKS_O_NOFOLLOW 		= 0x20000,
	FKS_O_CLOEXEC 		= 0x80000,
	FKS_O_NOATIME 		= 0x40000,
	FKS_O_SYNC 			= 0x101000,
	FKS_O_PATH 			= 0x200000,
	FKS_O_TMPFILE 		= 0x410000,

	FKS_O_TEXT			= 0x0000,
	FKS_O_BINARY		= 0x0000,
	FKS_O_RAW			= FKS_O_BINARY,
 #endif
};


// sys/stat.h
enum {
	FKS_S_IEXEC  = 0x0040,
	FKS_S_IWRITE = 0x0080,
	FKS_S_IREAD  = 0x0100,
	FKS_S_ISVTX  = 0x0200,	// linux
	FKS_S_ISGID  = 0x0400,	// linux
	FKS_S_ISUID  = 0x0800,	// linux
	FKS_S_IFIFO  = 0x1000,
	FKS_S_IFCHR  = 0x2000,
	FKS_S_IFDIR  = 0x4000,
	FKS_S_IFBLK  = 0x6000,
	FKS_S_IFREG  = 0x8000,
	FKS_S_IFLNK  = 0xa000,	// linux
	FKS_S_IFSOCK = 0xc000,	// linux
	FKS_S_IFMT	 = 0xf000,	/* File type mask */

	FKS_S_IRWXU	=	(FKS_S_IREAD | FKS_S_IWRITE | FKS_S_IEXEC),
	FKS_S_IXUSR	=	FKS_S_IEXEC,
	FKS_S_IWUSR	=	FKS_S_IWRITE,
	FKS_S_IRUSR	=	FKS_S_IREAD
};
#define	FKS_S_ISDIR(m)		(((m) & FKS_S_IFMT) == FKS_S_IFDIR)
#define	FKS_S_ISFIFO(m)		(((m) & FKS_S_IFMT) == FKS_S_IFIFO)
#define	FKS_S_ISCHR(m)		(((m) & FKS_S_IFMT) == FKS_S_IFCHR)
#define	FKS_S_ISBLK(m)		(((m) & FKS_S_IFMT) == FKS_S_IFBLK)
#define	FKS_S_ISREG(m)		(((m) & FKS_S_IFMT) == FKS_S_IFREG)

struct fks_stat {
  #ifdef FKS_WIN32
	fks_mode_t		st_mode;	/* FKS_S_??? */
	unsigned		st_w32attr;	/* Win32 file attributes.(fks-libc only) */
	fks_off_t		st_size;	/* File size (bytes) */
	fks_time_t		st_atime;	/* Accessed time */
	fks_time_t		st_mtime;	/* Modified time */
	fks_time_t		st_ctime;	/* Creation time */
  #else //elif FKS_LINUX
	fks_dev_t		st_dev;		/* Dummy (0) */
	fks_ino_t		st_ino;		/* Dummy (0) */
	fks_mode_t		st_mode;	/* FKS_S_??? */
	short			st_nlink;	/* Dummy (0) */
	short			st_uid;		/* Dummy (0) */
	short			st_gid;		/* Dummy (0) */
	fks_dev_t		st_rdev;	/* Dummy (0) */
	fks_off_t		st_size;	/* File size (bytes) */
	fks_time_t		st_atime;	/* Accessed time */
	fks_time_t		st_mtime;	/* Modified time */
	fks_time_t		st_ctime;	/* Creation time */
 #endif
};
typedef struct fks_stat fks_stat_t;

#if 0 // sys/utime.h
struct fks_utimbuf {
	fks_time_t	actime;
	fks_time_t	modtime;
};
#endif

// sys/fks_time.h
struct fks_timeval {
  long tv_sec;
  long tv_usec;
};
typedef struct fks_timeval	fks_timeval_t;


/* file path */
FKS_LIB_DECL (fks_io_rc_t)	fks_access(const char* fpath, int rdwrt) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)	fks_chdir (const char* fpath) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)  fks_chmod (const char* fpath, int mod) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)        fks_getcwd(char fpath[], int maxlen) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)  fks_mkdir (const char* fpath) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)	fks_rmdir (const char* fpath) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)	fks_unlink(const char* fpath) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)	fks_remove(const char* fpath) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)	fks_rename(const char* oldname, const char* newname) FKS_NOEXCEPT;

// sys/stat.h
FKS_LIB_DECL(fks_io_rc_t)	fks_stat (const char* fpath, fks_stat_t * fd) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_io_rc_t)	fks_chmod(const char* fpath, int mod) FKS_NOEXCEPT;
#if 0 // sys/utime.h
FKS_LIB_DECL(fks_io_rc_t) fks_utime(const char* fname, struct fks_utimbuf*) FKS_NOEXCEPT;
#endif

enum {
	FKS_FILE_ATTR_DIRECTORY = 0x00000010,
	FKS_FILE_ATTR_NORMAL	= 0x00000080,
};
FKS_LIB_DECL(unsigned int)	fks_fileAttr(const char* fpath) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_off64_t)	fks_fileSize(const char* fname) FKS_NOEXCEPT;
FKS_LIB_DECL(int)			fks_isDir(const char* fpath) FKS_NOEXCEPT;
FKS_LIB_DECL(int)    		fks_fileExist(const char* f) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)		fks_fileFullpath(char fpath[], size_t l, const char* s) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_io_rc_t)   fks_fileGetTime(const char* name, fks_time_t* pCrt, fks_time_t* pAcs, fks_time_t* pWrt) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_io_rc_t)	fks_fileSetTime(const char* name, fks_time_t creat, fks_time_t lastAcs, fks_time_t lastWrt) FKS_NOEXCEPT;	// use fh
FKS_LIB_DECL(fks_io_rc_t)	fks_fileMove(const char* srcname, const char* dstname, int overriteFlag);
FKS_LIB_DECL(void*)			fks_fileLoad(const char* fname, void* mem, size_t size, size_t* pReadSize) FKS_NOEXCEPT;	// use fh
FKS_LIB_DECL(void const*)	fks_fileSave(const char* fname, const void* mem, size_t size) FKS_NOEXCEPT;					// use fh

FKS_LIB_DECL (fks_io_rc_t)	fks_recursiveMkDir(const char* fpath) FKS_NOEXCEPT;				// use fks_fname.h
//FKS_LIB_DECL (fks_io_rc_t)	fks_recursiveRmDir(const char* fpath) FKS_NOEXCEPT;

#ifdef FKS_WIN32
FKS_LIB_DECL (char*)		fks_getExePath(char nameBuf[], size_t nameBufSize) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)		fks_getSystemDir(char nameBuf[], size_t nameBufSize) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)		fks_getWindowsDir(char nameBuf[], size_t nameBufSize) FKS_NOEXCEPT;
#endif

FKS_LIB_DECL (int)			fks_getTmpEnv(char tmpEnv[], size_t size);
FKS_LIB_DECL (char*)		fks_tmpFile(char name[], size_t size, const char* prefix, char const* suffix);
FKS_LIB_DECL (int)			fks_fileDateCmp(const char *lhs, const char *rhs);


// ============================================================================

#ifdef FKS_USE_WIN_API
FKS_LIB_DECL (fks_fh_t)     fks_fh_in(void) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_fh_t)     fks_fh_out(void) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_fh_t)		fks_fh_err(void) FKS_NOEXCEPT;
#define FKS_FH_IN			fks_fh_in()
#define FKS_FH_OUT       	fks_fh_out()
#define FKS_FH_ERR       	fks_fh_err()
#else
#define FKS_FH_IN			(0)
#define FKS_FH_OUT       	(1)
#define FKS_FH_ERR       	(2)
#endif

FKS_LIB_DECL (fks_fh_t)     fks_creat (const char* fname, int rdwrt) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_fh_t)     fks_open  (const char* fname, int openflags, int pmode) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)	fks_close (fks_fh_t fh) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_fh_t)     fks_dup   (fks_fh_t fh) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)	fks_eof   (fks_fh_t fh) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_off_t)    fks_lseek (fks_fh_t fh, fks_off_t ofs, int seekmode) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_off_t)    fks_tell  (fks_fh_t fh) FKS_NOEXCEPT;
FKS_LIB_DECL (ptrdiff_t) 	fks_read  (fks_fh_t fh, void* mem, size_t bytes) FKS_NOEXCEPT;
FKS_LIB_DECL (ptrdiff_t) 	fks_write (fks_fh_t fh, const void* mem, size_t bytes) FKS_NOEXCEPT;

FKS_LIB_DECL (fks_io_rc_t)  fks_commit    (fks_fh_t fh) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_off_t)    fks_filelength(fks_fh_t fh) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)	fks_setmode   (fks_fh_t fh, int bintextmode) FKS_NOEXCEPT;

FKS_LIB_DECL(fks_io_rc_t)	fks_fhGetTime(fks_fh_t h, fks_time_t* pCreat, fks_time_t* pLastAcs, fks_time_t* pLastWrt) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_io_rc_t)	fks_fhSetTime(fks_fh_t h, fks_time_t creat, fks_time_t lastAcs, fks_time_t lastWrt) FKS_NOEXCEPT;

#if 0
// sys/stat.h
FKS_LIB_DECL(fks_io_rc_t)	fks_fstat(fks_fh_t fh, fks_stat_t * fd) FKS_NOEXCEPT;
#endif

// sys/time.h
#define fks_timerisset(tvp)             ((tvp)->tv_sec || (tvp)->tv_usec)
#define fks_timercmp(tvp, uvp, cmp)      \
    (((tvp)->tv_sec !=  (uvp)->tv_sec) ? \
    ((tvp)->tv_sec  cmp (uvp)->tv_sec) : \
    ((tvp)->tv_usec cmp (uvp)->tv_usec))
#define fks_timerclear(tvp)             ((tvp)->tv_sec = (tvp)->tv_usec = 0)
FKS_LIB_DECL (fks_io_rc_t)	fks_futimes(fks_fh_t h, struct fks_timeval tv[2]) FKS_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  /* FKS_IO_H */
