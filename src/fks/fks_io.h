/*
 *  @file   fks_io.h
 *  @brief  System level I/O functions and types.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_IO_H_INCLUDED
#define FKS_IO_H_INCLUDED

#include <fks_common.h>
#include <fks_types.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) && !defined(FKS_UNUSE_LONGFNAME)
#define FKS_USE_LONGFNAME
#endif

// types.h
#ifdef FKS_WIN32
typedef intptr_t        fks_fh_t;
#else
typedef int             fks_fh_t;
#endif

typedef int             fks_io_rc_t;        // 0:OK -:error

#if 0
typedef int64_t         fks_time_t;         // nano sec.
typedef int64_t         fks_off_t;
typedef int64_t         fks_isize_t;
typedef int64_t         fks_off64_t;
#endif

typedef int64_t         fks_fpos64_t;
typedef unsigned short  fks_mode_t;

#ifndef FKS_WIN32
typedef short           fks_ino_t;
typedef unsigned        fks_dev_t;
//typedef int           fks_pid_t;
//typedef int           fks_sigset_t;
//typedef long          fks_ssize_t;
typedef unsigned int    fks_useconds_t;
#endif


// fcntl.h
#define FKS_O_RDONLY            0x00
#define FKS_O_WRONLY            0x01
#define FKS_O_RDWR              0x02
#define FKS_O_ACCMODE           0x03
#ifdef FKS_WIN32
 #define    FKS_O_APPEND        0x0008
 #define    FKS_O_CREAT         0x0100
 #define    FKS_O_TRUNC         0x0200
 #define    FKS_O_EXCL          0x0400
 #define    FKS_O_RANDOM        0x0010
 #define    FKS_O_SEQUENTIAL    0x0020
 #define    FKS_O_TEMPORARY     0x0040
 #define    FKS_O_NOINHERIT     0x0080
 #define    FKS_O_SHORT_LIVED   0x1000
 #define    FKS_O_TEXT          0x4000  // fks-lib not support
 #define    FKS_O_BINARY        0x8000
#else
 #define    FKS_O_CREAT         0x040
 #define    FKS_O_EXCL          0x080
 #define    FKS_O_TRUNC         0x200
 #define    FKS_O_APPEND        0x400
 #define    FKS_O_NOCTTY        0x100
 #define    FKS_O_NONBLOCK      0x800
 #define    FKS_O_DSYNC         0x1000
 #define    FKS_O_ASYNC         0x2000
 #define    FKS_O_DIRECT        0x4000
 #define    FKS_O_LARGEFILE     0x8000
 #define    FKS_O_DIRECTORY     0x10000
 #define    FKS_O_NOFOLLOW      0x20000
 #define    FKS_O_CLOEXEC       0x80000
 #define    FKS_O_NOATIME       0x40000
 #define    FKS_O_SYNC          0x101000
 #define    FKS_O_PATH          0x200000
 #define    FKS_O_TMPFILE       0x410000
 #define    FKS_O_TEXT          0x0000
 #define    FKS_O_BINARY        0x0000
#endif
#define     FKS_O_RAW           FKS_O_BINARY

// sys/stat.h
#define FKS_S_IEXEC     0x0040
#define FKS_S_IWRITE    0x0080
#define FKS_S_IREAD     0x0100
#define FKS_S_ISVTX     0x0200  // linux
#define FKS_S_ISGID     0x0400  // linux
#define FKS_S_ISUID     0x0800  // linux
#define FKS_S_IFIFO     0x1000
#define FKS_S_IFCHR     0x2000
#define FKS_S_IFDIR     0x4000
#define FKS_S_IFBLK     0x6000
#define FKS_S_IFREG     0x8000
#define FKS_S_IFLNK     0xa000  // linux
#define FKS_S_IFSOCK    0xc000  // linux
#define FKS_S_IFMT      0xf000  /* File type mask */
#define FKS_S_IRWXU     (FKS_S_IREAD | FKS_S_IWRITE | FKS_S_IEXEC)
#define FKS_S_IXUSR     FKS_S_IEXEC
#define FKS_S_IWUSR     FKS_S_IWRITE
#define FKS_S_IRUSR     FKS_S_IREAD
#define FKS_S_ISDIR(m)  (((m) & FKS_S_IFMT) == FKS_S_IFDIR)
#define FKS_S_ISFIFO(m) (((m) & FKS_S_IFMT) == FKS_S_IFIFO)
#define FKS_S_ISCHR(m)  (((m) & FKS_S_IFMT) == FKS_S_IFCHR)
#define FKS_S_ISBLK(m)  (((m) & FKS_S_IFMT) == FKS_S_IFBLK)
#define FKS_S_ISREG(m)  (((m) & FKS_S_IFMT) == FKS_S_IFREG)

#define FKS_S_EX_ERROR			0x8000  // io error.
#define FKS_S_EX_UNKOWN			0x4000  // readdir's unkown.
#define FKS_S_EX_NOTMATCH		0x2000  // need by fks_dirent. Directory for recursion not matching names.
#define FKS_S_EX_DOTORDOTDOT	0x1000	// "." ".."

#ifdef _WIN32
enum Fks_S_Win32Attr {
    FKS_S_W32_ReadOnly 			= 0x01,
    FKS_S_W32_Hidden 			= 0x02,
    FKS_S_W32_System   			= 0x04,
    FKS_S_W32_Volume    		= 0x08,				// for dos.
    FKS_S_W32_Directory 		= 0x10,
    FKS_S_W32_Archive 			= 0x20,
	FKS_S_W32_Device    		= 0x40,
	FKS_S_W32_Normal   			= 0x80,
	FKS_S_W32_Temporary 		= 0x100,
	FKS_S_W32_SparseFile   		= 0x200,
	FKS_S_W32_ReparsePoint		= 0x400,
	FKS_S_W32_Compressed		= 0x800,
	FKS_S_W32_Offline   		= 0x1000,
	FKS_S_W32_NoIndexed 		= 0x2000,
    FKS_S_W32_Encrypted			= 0x4000,
	FKS_S_W32_IntegritySystem	= 0x8000,
	FKS_S_W32_Virtual			= 0x10000,
	FKS_S_W32_NoScrubData		= 0x20000,
	FKS_S_W32_EA				= 0x40000,
	FKS_S_W32_RecallOnOpen		= 0x40000,
	FKS_S_W32_Pinned			= 0x80000,
	FKS_S_W32_Unpinned			= 0x100000,
	FKS_S_W32_RecallOnDataAcs	= 0x400000,
	FKS_S_W32_StrictlySequential= 0x20000000,
};
//#define FKS_S_W32ATTR(a)		((a)|((!((a) & 0x1d7) && ((a) & 0x20))<<7))
#endif

struct fks_stat {
    fks_isize_t     st_size;    /* File size (bytes) */
    fks_time_t      st_atime;   /* Accessed time */
    fks_time_t      st_mtime;   /* Modified time */
    fks_time_t      st_ctime;   /* Creation time */
    fks_mode_t      st_mode;    /* FKS_S_??? */
    unsigned short  st_ex_mode; /* fks-lib only : error,unkown... */

  #ifdef FKS_WIN32
    unsigned        st_native_attr; /* Win32 file attributes.(fks-libc only) */
  #else //elif FKS_LINUX
    fks_dev_t       st_dev;     /*  */
    fks_ino_t       st_ino;     /*  */
    short           st_nlink;   /*  */
    short           st_uid;     /*  */
    short           st_gid;     /*  */
    fks_dev_t       st_rdev;    /*  */
 #endif
};
typedef struct fks_stat fks_stat_t;

#if 0
// sys/fks_time.h
struct fks_timeval {
  long tv_sec;
  long tv_usec;
};
typedef struct fks_timeval  fks_timeval_t;
#endif

#if 0 // sys/utime.h
//struct fks_utimbuf {
//  fks_time_t  actime;
//  fks_time_t  modtime;
//};
#endif



// ============================================================================

#ifdef FKS_USE_WIN_API
FKS_LIB_DECL (fks_fh_t)     fks_fh_in(void) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_fh_t)     fks_fh_out(void) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_fh_t)     fks_fh_err(void) FKS_NOEXCEPT;
#define FKS_FH_IN           fks_fh_in()
#define FKS_FH_OUT          fks_fh_out()
#define FKS_FH_ERR          fks_fh_err()
#else
#define FKS_FH_IN           (0)
#define FKS_FH_OUT          (1)
#define FKS_FH_ERR          (2)
#endif

FKS_LIB_DECL (fks_fh_t)     fks_open  (char const* fname, int openflags, int pmode FKS_ARG_INI(0777)) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_fh_t)     fks_creat (char const* fname, int pmode FKS_ARG_INI(0777)) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_off_t)    fks_lseek (fks_fh_t fh, fks_off_t ofs, int seekmode FKS_ARG_INI(0)) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)  fks_close (fks_fh_t fh) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_fh_t)     fks_dup   (fks_fh_t fh) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)  fks_eof   (fks_fh_t fh) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_off_t)    fks_tell  (fks_fh_t fh) FKS_NOEXCEPT;
FKS_LIB_DECL (ptrdiff_t)    fks_read  (fks_fh_t fh, void* mem, size_t bytes) FKS_NOEXCEPT;
FKS_LIB_DECL (ptrdiff_t)    fks_write (fks_fh_t fh, void const* mem, size_t bytes) FKS_NOEXCEPT;

FKS_LIB_DECL (fks_io_rc_t)  fks_commit    (fks_fh_t fh) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_isize_t)  fks_filelength(fks_fh_t fh) FKS_NOEXCEPT;

FKS_LIB_DECL(fks_io_rc_t)   fks_fhGetTime(fks_fh_t h, fks_time_t* pCreat, fks_time_t* pLastAcs, fks_time_t* pLastWrt) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_io_rc_t)   fks_fhSetTime(fks_fh_t h, fks_time_t creat, fks_time_t lastAcs, fks_time_t lastWrt) FKS_NOEXCEPT;

#if 0
// sys/stat.h
FKS_LIB_DECL(fks_io_rc_t)   fks_fstat(fks_fh_t fh, fks_stat_t * fd) FKS_NOEXCEPT;
#endif

// sys/time.h
#if 0
#define fks_timerisset(tvp)             ((tvp)->tv_sec || (tvp)->tv_usec)
#define fks_timercmp(tvp, uvp, cmp)      \
    (((tvp)->tv_sec !=  (uvp)->tv_sec) ? \
    ((tvp)->tv_sec  cmp (uvp)->tv_sec) : \
    ((tvp)->tv_usec cmp (uvp)->tv_usec))
#define fks_timerclear(tvp)             ((tvp)->tv_sec = (tvp)->tv_usec = 0)
FKS_LIB_DECL (fks_io_rc_t)  fks_futimes(fks_fh_t h, struct fks_timeval tv[2]) FKS_NOEXCEPT;
#endif


// ============================================================================

/* file path */
FKS_LIB_DECL (fks_io_rc_t)  fks_access(char const* fpath, int rdwrt) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)  fks_chdir (char const* fpath) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)  fks_chmod (char const* fpath, int mod) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)        fks_getcwd(char fpath[], int maxlen) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)  fks_mkdir (char const* fpath, int pmode FKS_ARG_INI(0777)) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)  fks_rmdir (char const* fpath) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)  fks_unlink(char const* fpath) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)  fks_remove(char const* fpath) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_io_rc_t)  fks_rename(char const* oldname, char const* newname) FKS_NOEXCEPT;

// sys/stat.h
FKS_LIB_DECL(fks_io_rc_t)   fks_stat (char const* fpath, fks_stat_t * fd) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_io_rc_t)   fks_chmod(char const* fpath, int mod) FKS_NOEXCEPT;
#if 0 // sys/utime.h
FKS_LIB_DECL(fks_io_rc_t)   fks_utime(char const* fname, struct fks_utimbuf*) FKS_NOEXCEPT;
#endif

enum {
 #ifdef FKS_USE_WIN_API
    FKS_FILE_ATTR_DIRECTORY = 0x00000010,
    FKS_FILE_ATTR_NORMAL    = 0x00000080,
 #else
    FKS_FILE_ATTR_DIRECTORY = FKS_S_IFDIR,
    FKS_FILE_ATTR_NORMAL    = 0x00000080,
 #endif
};
FKS_LIB_DECL(unsigned int)  fks_fileAttr(char const* fpath) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_off64_t)   fks_fileSize(char const* fname) FKS_NOEXCEPT;
FKS_LIB_DECL(int)           fks_isDir(char const* fpath) FKS_NOEXCEPT;
FKS_LIB_DECL(int)           fks_fileExist(char const* f) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)        fks_fileFullpath(char fpath[], size_t l, char const* s) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_io_rc_t)   fks_fileGetTime(char const* name, fks_time_t* pCrt, fks_time_t* pAcs, fks_time_t* pWrt) FKS_NOEXCEPT;
FKS_LIB_DECL(fks_io_rc_t)   fks_fileSetTime(char const* name, fks_time_t creat, fks_time_t lastAcs, fks_time_t lastWrt) FKS_NOEXCEPT;   // use fh
FKS_LIB_DECL(fks_io_rc_t)   fks_fileMove(char const* srcname, char const* dstname, int overriteFlag);
FKS_LIB_DECL(fks_io_rc_t)   fks_fileCopy(char const* srcname, char const* dstname, int overriteFlag);
FKS_LIB_DECL(void*)         fks_fileLoad(char const* fname, void* mem, size_t size, size_t* pReadSize) FKS_NOEXCEPT;    // use fh
FKS_LIB_DECL(void const*)   fks_fileSave(char const* fname, void const* mem, size_t size) FKS_NOEXCEPT;                 // use fh

FKS_LIB_DECL (fks_io_rc_t)  fks_recursiveMkDir(char const* fpath) FKS_NOEXCEPT;             // use fks_path.h
//FKS_LIB_DECL (fks_io_rc_t) fks_recursiveRmDir(char const* fpath) FKS_NOEXCEPT;

#ifdef FKS_WIN32
FKS_LIB_DECL (char*)        fks_getExePath(char nameBuf[], size_t nameBufSize) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)        fks_getSystemDir(char nameBuf[], size_t nameBufSize) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)        fks_getWindowsDir(char nameBuf[], size_t nameBufSize) FKS_NOEXCEPT;
#endif

FKS_LIB_DECL (int)          fks_getTmpEnv(char tmpEnv[], size_t size);
FKS_LIB_DECL (char*)        fks_tmpFile(char name[], size_t size, char const* prefix, char const* suffix FKS_ARG_INI(NULL));
//FKS_LIB_DECL (int)        fks_fileDateCmp(char const *lhs, char const *rhs);

#ifdef __cplusplus
}
#endif


#endif  /* FKS_IO_H_INCLUDED */
