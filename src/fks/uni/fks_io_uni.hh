/**
 *  @file   fks_io_uni.hh
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#include <fks/fks_io.h>
#include <fks/fks_path.h>
#include <fks/fks_perfcnt.h>
#include <fks/fks_assert_ex.h>
#include <fks/fks_time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <fnmatch.h>


#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL (fks_fh_t)
fks_open  (char const* fname, int openflags, int pmode) FKS_NOEXCEPT
{
    return open(fname, openflags, pmode);
}


FKS_LIB_DECL (fks_fh_t)
fks_creat (char const* fname, int pmode) FKS_NOEXCEPT
{
    return creat(fname, pmode);
}


FKS_LIB_DECL (fks_off_t)
fks_lseek (fks_fh_t fh, fks_off_t ofs, int seekmode) FKS_NOEXCEPT
{
    return lseek(fh, ofs, seekmode);
}


FKS_LIB_DECL (fks_io_rc_t)
fks_close (fks_fh_t fh) FKS_NOEXCEPT
{
    return close(fh);
}


FKS_LIB_DECL (fks_fh_t)
fks_dup   (fks_fh_t fh) FKS_NOEXCEPT
{
    return dup(fh);
}


FKS_LIB_DECL (fks_io_rc_t)
fks_eof   (fks_fh_t fh) FKS_NOEXCEPT
{
    fks_isize_t l;
    FKS_ARG_ASSERT(1, fh != ((fks_fh_t)-1));
    l = fks_filelength(fh);
    return fks_tell(fh) >= l;
}


FKS_LIB_DECL (fks_off_t)
fks_tell  (fks_fh_t fh) FKS_NOEXCEPT
{
    FKS_ARG_ASSERT(1, fh != ((fks_fh_t)-1));
    return fks_lseek(fh, 0, 1/*FILE_CURRENT*/);
}


FKS_LIB_DECL (ptrdiff_t)
fks_read  (fks_fh_t fh, void* mem, size_t bytes) FKS_NOEXCEPT
{
    return read(fh, mem, bytes);
}


FKS_LIB_DECL (ptrdiff_t)
fks_write (fks_fh_t fh, void const* mem, size_t bytes) FKS_NOEXCEPT
{
    return write(fh, mem, bytes);
}

#if 0
FKS_LIB_DECL (fks_io_rc_t)
fks_commit    (fks_fh_t fh) FKS_NOEXCEPT
{
    return commit(fh);
}
#endif

FKS_LIB_DECL (fks_isize_t)
fks_filelength(fks_fh_t fh) FKS_NOEXCEPT
{
    struct stat st;
    int    rc = fstat(fh, &st);
    return (rc < 0) ? 0 : st.st_size;
}


FKS_LIB_DECL(fks_io_rc_t)
fks_fhGetTimespec(fks_fh_t fh, fks_timespec* pCrt, fks_timespec* pAcs, fks_timespec* pWrt) FKS_NOEXCEPT
{
    struct stat st;
    int    rc = fstat(fh, &st);
    if (rc >= 0) {
	 #if defined FKS_APPLE
		if (pAcs) *pAcs = *(fks_timespec const*)&st.st_atimespec;
		if (pWrt) *pWrt = *(fks_timespec const*)&st.st_mtimespec;
		if (pCrt) *pCrt = *(fks_timespec const*)&st.st_ctimespec;
	 #elif 1 //defined FKS_LINUX
		if (pAcs) *pAcs = *(fks_timespec const*)&st.st_atim;
		if (pWrt) *pWrt = *(fks_timespec const*)&st.st_mtim;
		if (pCrt) *pCrt = *(fks_timespec const*)&st.st_ctim;
	 #else
		if (pAcs) pAcs->tv_sec = st.st_atime, pAct->tv_nsec = 0;
		if (pWrt) pWrt->tv_sec = st.st_mtime, pWrt->tv_nsec = 0;
		if (pCrt) pCrt->tv_sec = st.st_ctime, pCrt->tv_nsec = 0;
	 #endif
    } else {
        if (pAcs) pAcs->tv_sec = 0, pAcs->tv_nsec = 0;
        if (pWrt) pWrt->tv_sec = 0, pWrt->tv_nsec = 0;
        if (pCrt) pCrt->tv_sec = 0, pCrt->tv_nsec = 0;
    }
    return rc;
}


FKS_LIB_DECL(fks_io_rc_t)
fks_fhSetTimespec(fks_fh_t h, fks_timespec const* pCrt, fks_timespec const* pAcs, fks_timespec const* pWrt) FKS_NOEXCEPT
{
}


FKS_LIB_DECL (fks_io_rc_t)
fks_access(char const* fpath, int rdwrt) FKS_NOEXCEPT
{
    return access(fpath, rdwrt);
}


FKS_LIB_DECL (fks_io_rc_t)
fks_chdir (char const* fpath) FKS_NOEXCEPT
{
    return chdir(fpath);
}


FKS_LIB_DECL (fks_io_rc_t)
fks_chmod (char const* fpath, int mod) FKS_NOEXCEPT
{
    return chmod(fpath, mod);
}


FKS_LIB_DECL (char*)
fks_getcwd(char fpath[], int maxlen) FKS_NOEXCEPT
{
    return getcwd(fpath, maxlen);
}


FKS_LIB_DECL (fks_io_rc_t)
fks_mkdir (char const* fpath, int pmode) FKS_NOEXCEPT
{
    return mkdir(fpath, pmode);
}


FKS_LIB_DECL (fks_io_rc_t)
fks_rmdir (char const* fpath) FKS_NOEXCEPT
{
    return rmdir(fpath);
}


FKS_LIB_DECL (fks_io_rc_t)
fks_unlink(char const* fpath) FKS_NOEXCEPT
{
    return unlink(fpath);
}


FKS_LIB_DECL (fks_io_rc_t)
fks_remove(char const* fpath) FKS_NOEXCEPT
{
    return remove(fpath);
}


FKS_LIB_DECL (fks_io_rc_t)
fks_rename(char const* oldname, char const* newname) FKS_NOEXCEPT
{
    return rename(oldname, newname);
}

FKS_STATIC_DECL(void) fks_stat_t_from_stat(fks_stat_t* d, struct stat const* s) FKS_NOEXCEPT;

FKS_LIB_DECL(fks_io_rc_t)
fks_stat(char const* fpath, fks_stat_t * fd) FKS_NOEXCEPT
{
    struct stat st;
    memset(&st, 0, sizeof st);
    int    rc = stat(fpath, &st);
    fks_stat_t_from_stat(fd, &st);
    return rc;
}

FKS_LIB_DECL(fks_io_rc_t)
fks_lstat(char const* fpath, fks_stat_t * fd) FKS_NOEXCEPT
{
    struct stat st;
    memset(&st, 0, sizeof st);
    int    rc = lstat(fpath, &st);
    fks_stat_t_from_stat(fd, &st);
    return rc;
}


FKS_STATIC_DECL(void) fks_stat_t_from_stat(fks_stat_t* d, struct stat const* s) FKS_NOEXCEPT
{
    d->st_size      = s->st_size;    /* File size (bytes) */
 #if defined FKS_APPLE
	d->st_atimespec = *(fks_timespec const*)&s->st_atimespec;
	d->st_mtimespec = *(fks_timespec const*)&s->st_mtimespec;
	d->st_ctimespec = *(fks_timespec const*)&s->st_ctimespec;
 #elif 1 //defined FKS_LINUX
	d->st_atimespec = *(fks_timespec const*)&s->st_atim;
	d->st_mtimespec = *(fks_timespec const*)&s->st_mtim;
	d->st_ctimespec = *(fks_timespec const*)&s->st_ctim;
 #else
	d->st_atimespec.tv_sec  = s->st_atime; d->st_atimespec.tv_nsec = 0;
	d->st_mtimespec.tv_sec  = s->st_mtime; d->st_mtimespec.tv_nsec = 0;
	d->st_ctimespec.tv_sec  = s->st_ctime; d->st_ctimespec.tv_nsec = 0;
 #endif
    d->st_mode      = s->st_mode;
    d->st_dev       = s->st_dev;
    d->st_ino       = s->st_ino;
    d->st_nlink 	= s->st_nlink;
    d->st_uid       = s->st_uid;
    d->st_gid       = s->st_gid;
    d->st_rdev      = s->st_rdev;
    d->st_native_mode = s->st_mode;
    d->st_ex_mode   = 0;
}


FKS_LIB_DECL(unsigned int)
fks_fileAttr(char const* fpath) FKS_NOEXCEPT
{
    struct stat st;
    int    rc = stat(fpath, &st);
    return (rc < 0) ? 0 : st.st_mode;
}


FKS_LIB_DECL(fks_off64_t)
fks_fileSize(char const* fpath) FKS_NOEXCEPT
{
    struct stat st;
    int    rc = stat(fpath, &st);
    return (rc < 0) ? 0 : st.st_size;
}


FKS_LIB_DECL(int)
fks_isDir(char const* fpath) FKS_NOEXCEPT
{
    struct stat st;
    int    rc = stat(fpath, &st);
    return (rc < 0) ? 0 : ((st.st_mode & S_IFMT)  == S_IFDIR);
}


FKS_LIB_DECL(int)
fks_fileExist(char const* fpath) FKS_NOEXCEPT
{
    struct stat st;
    int    rc = stat(fpath, &st);
    return (rc < 0) ? 0 : 1;
}


FKS_LIB_DECL (char*)
fks_fileFullpath(char fpath[], size_t l, char const* s) FKS_NOEXCEPT
{
    char cur[FKS_PATH_MAX] = {0};
    ;
    if (fks_getcwd(cur, FKS_PATH_MAX) != NULL) {
        fks_pathFullpath(fpath, l, s, cur);
        return fpath;
    } else {
        fks_pathCpy(fpath, l, s);
        return NULL;
    }
}


FKS_LIB_DECL(fks_io_rc_t)
fks_fileGetTimespec(char const* name, fks_timespec* pCrt, fks_timespec* pAcs, fks_timespec* pWrt) FKS_NOEXCEPT
{
    fks_stat_t st;
    int rc = fks_stat(name, &st);
    if (pAcs) *pAcs = st.st_atimespec;   /* Accessed time */
    if (pWrt) *pWrt = st.st_mtimespec;   /* Modified time */
    if (pCrt) *pCrt = st.st_ctimespec;   /* Creation time */
    return rc;
}


FKS_LIB_DECL(fks_io_rc_t)
fks_fileSetTimespec(char const* name, fks_timespec const* pCrt, fks_timespec const* pAcs, fks_timespec const* pWrt) FKS_NOEXCEPT
{
}


FKS_LIB_DECL(fks_io_rc_t)
fks_fileMove(char const* srcname, char const* dstname, int overriteFlag) FKS_NOEXCEPT
{
}


FKS_LIB_DECL(fks_io_rc_t)
fks_fileCopy(char const* srcname, char const* dstname, int overriteFlag) FKS_NOEXCEPT
{
}


FKS_STATIC_DECL (int) fks_recursiveMkDir_subr(const char* name, int pmode);

/** Extension of mkdir, making directories on the way
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_recursiveMkDir(const char *name, int pmode) FKS_NOEXCEPT
{
    if (fks_mkdir(name, pmode) == 0)
        return 0;   // ok.

    return fks_recursiveMkDir_subr(name, pmode);
}

FKS_STATIC_DECL (int)
fks_recursiveMkDir_subr(const char* name, int pmode)
{
    char    nm[ FKS_PATH_MAX_URL + 1 ];
    char*   e;
    char*   s;

    fks_pathCpy(nm, FKS_PATH_MAX_URL, name);
    e = nm + fks_pathLen(nm);
    do {
        s = fks_pathBaseName(nm);
        if (s <= nm)
            return -1; // NG
        --s;
        *s = 0;
    } while (fks_mkdir(nm, pmode) != 0);
    do {
        *s    = FKS_PATH_SEP_CHR;
        s    += fks_pathLen(s);
        if (s >= e)
            return fks_mkdir(nm, pmode);
    } while (fks_mkdir(nm, pmode) == 0);
    return -1; // NG
}



FKS_LIB_DECL (char*)
fks_tmpFile(char path[], size_t size, char const* prefix, char const* suffix) FKS_NOEXCEPT
{
    int         h;
    uint64_t    tmr;    // time_t   tmr;
    size_t      l;
    unsigned    idx;
    unsigned    pid;
    char        tmpd[ FKS_PATH_MAX + 1];

    FKS_ARG_PTR_ASSERT(1, path);
    l = strlen(path);
    FKS_ARG_ASSERT(2, l + size >= 20);
    if (!path || size < l + 20) {
        return NULL;
    }

    if (!prefix)
        prefix = "";
    if (!suffix)
        suffix = "";

    FKS_ASSERT(strlen(prefix) + strlen(suffix) + 20 < size);
    if (strlen(prefix) + strlen(suffix) + 20 >= size) {
        return NULL;
    }
    tmpd[0] = 0;
    tmpd[FKS_PATH_MAX] = 0;
    if (l > 0) {
        fks_pathDelLastSep(path);
        if (fks_isDir(path)) {
            fks_pathCpy(tmpd, FKS_PATH_MAX, path);
        }
    }
    if (tmpd[0] == 0)
        fks_pathCpy(tmpd, FKS_PATH_MAX, "/tmp");

    pid = getpid();
    tmr = fks_getCurrentGlobalTime() / 997 + pid * 991;
    pid = ((pid / 41) * 17 + (pid % 41)*0x10003) ^ ( 0x00102101);
    tmr *= 16;
    idx = 0;
    do {
        unsigned ti;
        ++idx;
        ti = (unsigned)(tmr + idx);
        snprintf(path, size-1, "%s/%s%08x%08x%s", tmpd, prefix, pid, ti, suffix);
        path[size-1] = 0;
        h = fks_open(path,  O_CREAT|O_WRONLY|O_EXCL, 0744);
    } while (h < 0 && idx < 16);
    if (h < 0)
        return NULL;
    close(h);
    return path;
}


#ifdef __cplusplus
}
#endif
