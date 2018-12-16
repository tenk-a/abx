/**
 *  @file   fks_priv_longfname_from_cs.hh
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#include <fks_common.h>

#ifdef FKS_WIN32

#include <fks_io.h>
#include <fks_alloca.h>
#include <fks_malloc.h>
#include <fks_assert_ex.h>
#include <fks_path.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fks_io_priv_w32.h"
//#include <fks_misc.h>


#include <windows.h>
#ifdef _MSC_VER
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#define snprintf    _snprintf
#endif

//#include <fks_mbswcs_var.h>
//#include <fks_inl_wrap_mac.h>

#ifdef __cplusplus
extern "C" {
#endif

/** file access check
 * @param fpath     target file
 * @param mode      0:exist 4:read only  // (not support 2:write only 6:read/write)
 * @retval 0        exist
 * @retval -1       not exist
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_access(const char* fpath, int mode) FKS_NOEXCEPT
{
    unsigned attr;
    FKS_ARG_PTR_ASSERT(1, fpath);
    FKS_ARG_ASSERT(2, (mode & ~6) == 0);
 #ifdef FKS_USE_LONGFNAME
    {
        wchar_t* fpathW;
        FKS_LONGFNAME_FROM_CS_INI(1);
        FKS_LONGFNAME_FROM_CS(0, fpathW, fpath);
        attr = GetFileAttributesW(fpathW);
    }
 #else
    attr = GetFileAttributesA(fpath);
 #endif
    if (attr == ~0U)
        return -1;
    if (mode == 4)
        return (attr & 1/*FILE_ATTRIBUTE_READONLY*/) - 1;
    return 0;
}


/** change current directory
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_chdir (const char* fpath) FKS_NOEXCEPT
{
 #ifdef FKS_USE_LONGFNAME
    wchar_t* fpathW;
    FKS_LONGFNAME_FROM_CS_INI(1);
    FKS_ARG_PTR_ASSERT(1, fpath);
    FKS_LONGFNAME_FROM_CS(0, fpathW, fpath);
    return SetCurrentDirectoryW(fpathW) ? 0 : -1;
 #else
    FKS_ARG_PTR_ASSERT(1, fpath);
    return SetCurrentDirectoryA(fpath) ? 0 : -1;
 #endif
}


/** change file mode(attributes)
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_chmod(const char* fpath, int mod) FKS_NOEXCEPT
{
 #ifdef FKS_USE_LONGFNAME
    wchar_t* fpathW;
    FKS_LONGFNAME_FROM_CS_INI(1);
    FKS_ARG_PTR_ASSERT(1, fpath);
    FKS_LONGFNAME_FROM_CS(0, fpathW, fpath);
    return SetFileAttributesW(fpathW, FKS_STATMODE_TO_W32FATTR(mod)) ? 0 : -1;
 #else
    FKS_ARG_PTR_ASSERT(1, fpath);
    return SetFileAttributesA(fpath, FKS_STATMODE_TO_W32FATTR(mod)) ? 0 : -1;
 #endif
}


/** get current working directory
 */
FKS_LIB_DECL (char*)
fks_getcwd(char dir[], int capa) FKS_NOEXCEPT
{
 #ifdef FKS_USE_LONGFNAME
    void*    freep = NULL;
    wchar_t* dirW;
    unsigned rc;
    int      wcapa = capa * sizeof(wchar_t) + 1;
    FKS_ARG_PTR_ASSERT(1, dir);

    dirW = (wchar_t*)fks_alloca(wcapa);
    if (!dirW) {
        freep = fks_malloc(wcapa);
        dirW  = (wchar_t*)freep;
    }
    rc = GetCurrentDirectoryW(capa, dirW);
    if (rc) {
        rc = FKS_MBS_FROM_WCS(dir, capa, dirW, rc);
        if (rc && rc < (unsigned)capa) {
            dir[rc] = '\0';
            fks_free(freep);
            return dir;
        }
    }
    fks_free(freep);
    return NULL;
 #else
    FKS_ARG_PTR_ASSERT(1, dir);
    FKS_ARG_ASSERT(2, capa > 0);
    return GetCurrentDirectoryA(capa, dir) ? dir : 0 ;
 #endif
}


/** make directory
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_mkdir (const char* fpath, int dmy_pmode) FKS_NOEXCEPT
{
 #ifdef FKS_USE_LONGFNAME
    wchar_t* fpathW;
    FKS_LONGFNAME_FROM_CS_INI(1);
    FKS_ARG_PTR_ASSERT(1, fpath);
    FKS_LONGFNAME_FROM_CS(0, fpathW, fpath);
    return CreateDirectoryW(fpathW, NULL) ? 0 : -1;
 #else
    FKS_ARG_PTR_ASSERT(1, fpath);
    return CreateDirectoryA(fpath,NULL) ? 0 : -1;
 #endif
}


/** remove directory
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_rmdir (const char* fpath) FKS_NOEXCEPT
{
 #ifdef FKS_USE_LONGFNAME
    wchar_t* fpathW;
    FKS_LONGFNAME_FROM_CS_INI(1);
    FKS_ARG_PTR_ASSERT(1, fpath);
    FKS_LONGFNAME_FROM_CS(0, fpathW, fpath);
    return RemoveDirectoryW( fpathW ) ? 0 : -1;
 #else
    FKS_ARG_PTR_ASSERT(1, fpath);
    return RemoveDirectoryA( fpath ) ? 0 : -1;
 #endif
}


/** delete file
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_remove (const char* fpath) FKS_NOEXCEPT
{
 #ifdef FKS_USE_LONGFNAME
    wchar_t* fpathW;
    FKS_LONGFNAME_FROM_CS_INI(1);
    FKS_ARG_PTR_ASSERT(1, fpath);
    FKS_LONGFNAME_FROM_CS(0, fpathW, fpath);
    return DeleteFileW( fpathW ) ? 0 : -1;
 #else
    FKS_ARG_PTR_ASSERT(1, fpath);
    return DeleteFileA( fpath ) ? 0 : -1;
 #endif
}


/** rename old file to new file
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_rename(const char* oldfname, const char* newfname) FKS_NOEXCEPT
{
 #ifdef FKS_USE_LONGFNAME
    wchar_t* oldfnameW;
    wchar_t* newfnameW;
    FKS_LONGFNAME_FROM_CS_INI(2);
    FKS_ARG_PTR_ASSERT(1, oldfname);
    FKS_ARG_PTR_ASSERT(2, newfname);
    FKS_LONGFNAME_FROM_CS(0, oldfnameW, oldfname);
    FKS_LONGFNAME_FROM_CS(1, newfnameW, newfname);
    return MoveFileW( oldfnameW, newfnameW ) ? 0 : -1;
 #else
    FKS_ARG_PTR_ASSERT(1, oldfname);
    FKS_ARG_PTR_ASSERT(2, newfname);
    return MoveFileA( oldfname, newfname ) ? 0 : -1;
 #endif
}


/** create file
 */
FKS_LIB_DECL (fks_fh_t)
fks_creat (const char* fname, int pmode) FKS_NOEXCEPT
{
    return fks_open(fname, FKS_O_CREAT, pmode);
}


/** open file subrutine
 */
FKS_LIB_DECL (fks_fh_t)
fks_open(const char* fname, int mode, int pmode) FKS_NOEXCEPT
{
    static DWORD const s_desiredAcs[4] = {
        GENERIC_READ,
        GENERIC_WRITE,
        GENERIC_READ| GENERIC_WRITE,
        GENERIC_READ| GENERIC_WRITE,
    };
    DWORD       dwAcs   = s_desiredAcs[ mode & FKS_O_ACCMODE ]
                          | ((mode & FKS_O_CREAT) ? GENERIC_WRITE : 0);
    DWORD       dwCreat = OPEN_EXISTING;
    DWORD       dwShr   = FILE_SHARE_READ;
    DWORD       dwAttr  = FILE_ATTRIBUTE_NORMAL;

    if (mode & FKS_O_RANDOM)
        dwAttr |= FILE_FLAG_RANDOM_ACCESS;
    if (mode & FKS_O_SEQUENTIAL)
        dwAttr |= FILE_FLAG_SEQUENTIAL_SCAN;
    if ((pmode&(7<<6)) == FKS_S_IREAD) {
        // dwAttr &= ~FILE_ATTRIBUTE_NORMAL;
        dwAttr |= FILE_ATTRIBUTE_READONLY;
    }
    if (mode & FKS_O_CREAT) {
        mode   &= ~FKS_O_APPEND;
        dwCreat = CREATE_ALWAYS;
        if (mode & FKS_O_EXCL)
            dwCreat = CREATE_NEW;
    }
    if (mode & FKS_O_TRUNC) {
        mode   &= ~FKS_O_APPEND;
        dwCreat = TRUNCATE_EXISTING;
    }
    if (mode & FKS_O_APPEND) {
        dwCreat = OPEN_EXISTING;
    }
    {
        HANDLE  w32hdl;
     #ifdef FKS_USE_LONGFNAME
        wchar_t*            fnameW;
        FKS_LONGFNAME_FROM_CS_INI(1);
        FKS_LONGFNAME_FROM_CS(0, fnameW, fname);
        w32hdl  = CreateFileW(fnameW, dwAcs, dwShr, 0, dwCreat, dwAttr, 0);
     #else
        w32hdl  = CreateFileA(fname, dwAcs, dwShr, 0, dwCreat, dwAttr, 0);
     #endif
        FKS_ASSERT((w32hdl > (HANDLE)2 || w32hdl == (HANDLE)INVALID_HANDLE_VALUE)
            && "This Library was the premise that HANDLE had a bigger than 2, but failed.");
     #if 0
        if ((mode & FKS_O_APPEND) && w32hdl != (HANDLE)INVALID_HANDLE_VALUE ) {
            fks_lseek((fks_fh_t)w32hdl, 0, 2);
        }
     #endif
        return (fks_fh_t)w32hdl;
    }
}


/** close file
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_close(fks_fh_t h) FKS_NOEXCEPT
{
    if (h != ((fks_fh_t)-1)) {
        CloseHandle(FKS_PRIV_FH2WH(h));
    }
    return 0;
}


/** duplicate file handle.
 */
FKS_LIB_DECL (fks_fh_t)     fks_dup(fks_fh_t fh) FKS_NOEXCEPT
{
    HANDLE  newHdl = (HANDLE)INVALID_HANDLE_VALUE;
    FKS_ARG_ASSERT(1, fh != ((fks_fh_t)-1));
    DuplicateHandle(GetCurrentProcess()
                            , (HANDLE ) fh
                            , GetCurrentProcess()
                            , (HANDLE*) &newHdl
                            , 0, 0, DUPLICATE_SAME_ACCESS );
    return (fks_fh_t)newHdl;
}

#if 0
/** Is end of file?
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_eof(fks_fh_t fh) FKS_NOEXCEPT
{
    fks_isize_t l;
    FKS_ARG_ASSERT(1, fh != ((fks_fh_t)-1));
    l = fks_filelength(fh);
    return fks_tell(fh) >= l;
}
#endif

/** logical file seek
 */
FKS_LIB_DECL (fks_off_t)
fks_lseek(fks_fh_t h, fks_off_t offset, int seekmode) FKS_NOEXCEPT
{
  #if !defined(FKS_NO_INT64) && (_WIN32_WINNT >= 0x0500 || FKS_WIN32 >= 0x0500) && !defined(FKS_USE_OLD_WIN32_API)
    FKS_STATIC_ASSERT(sizeof(fks_off_t) == sizeof(int64_t));
    FKS_ARG_ASSERT(1, h != ((fks_fh_t)-1));
    return SetFilePointerEx(FKS_PRIV_FH2WH(h)
                                , *(LARGE_INTEGER*)&offset
                                ,  (LARGE_INTEGER*)&offset, seekmode)
            ? offset : 0;
  #else
    return SetFilePointer(FKS_PRIV_FH2WH(h), (long)offset, 0, (DWORD)seekmode);
  #endif
}


/** get current seek positon.
 */
FKS_LIB_DECL (fks_off_t)
fks_tell(fks_fh_t h) FKS_NOEXCEPT
{
    FKS_ARG_ASSERT(1, h != ((fks_fh_t)-1));
    return fks_lseek(h, 0, 1/*FILE_CURRENT*/);
}


/** read file
 */
FKS_LIB_DECL (ptrdiff_t)
fks_read(fks_fh_t h, void* b, size_t sz) FKS_NOEXCEPT
{
    unsigned long r = 0;
    FKS_ARG_ASSERT(1, h != ((fks_fh_t)-1));
    FKS_ARG_PTR_ASSERT(2, b);
    if (sz && !(ReadFile(FKS_PRIV_FH2WH(h), b, sz, &r, 0)))
        r = 0;
    return r;
}


/** write file
 */
FKS_LIB_DECL (ptrdiff_t)    fks_write(fks_fh_t h, const void* b, size_t sz) FKS_NOEXCEPT
{
    unsigned long r=0;
    FKS_ARG_ASSERT(1, h != ((fks_fh_t)-1));
    FKS_ARG_PTR_ASSERT(2, b);
    if (sz && !(WriteFile(FKS_PRIV_FH2WH(h),b,sz,&r,0)))
        r = 0;
    return r;
}


/** flush file buffers.
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_commit(fks_fh_t h) FKS_NOEXCEPT
{
    FKS_ARG_ASSERT(1, h != ((fks_fh_t)-1));
    return FlushFileBuffers(FKS_PRIV_FH2WH(h)) ? 0 : -1;
}


/** get file size.
 */
FKS_LIB_DECL (fks_isize_t)
fks_filelength(fks_fh_t h) FKS_NOEXCEPT
{
  #if !defined(FKS_NO_INT64) && (_WIN32_WINNT>=0x0500 || FKS_WIN32>=0x0500) && !defined(FKS_USE_OLD_WIN32_API)
    FKS_STATIC_ASSERT( sizeof(fks_isize_t) == sizeof(int64_t) );
    uint64_t l = 0;
    FKS_ARG_ASSERT(1, h != ((fks_fh_t)-1));
    return GetFileSizeEx(FKS_PRIV_FH2WH(h), (LARGE_INTEGER*)&l) ? l : 0;
  #else
    unsigned long m = 0, l;
    l = GetFileSize(FKS_PRIV_FH2WH(h), (DWORD*)&m);
    return m ? -1 : l;
  #endif
}


/** Get file status.
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_stat(const char* fname, fks_stat_t* st) FKS_NOEXCEPT
{
    WIN32_FILE_ATTRIBUTE_DATA   atr = {0};
    int     rc;

 #ifdef FKS_USE_LONGFNAME
    wchar_t* fnameW;
    FKS_LONGFNAME_FROM_CS_INI(1);
    FKS_ARG_PTR_ASSERT(1, fname);
    FKS_LONGFNAME_FROM_CS(0, fnameW, fname);
    rc = GetFileAttributesExW(fnameW, GetFileExInfoStandard, &atr);
 #else
    FKS_ARG_PTR_ASSERT(1, fname);
    rc = GetFileAttributesExA(fname, GetFileExInfoStandard, &atr);
 #endif
    FKS_ARG_PTR_ASSERT(2, st);
  #if 0
    st->st_dev      = 0;
    st->st_ino      = 0;
    st->st_nlink    = 0;
    st->st_uid      = 0;
    st->st_gid      = 0;
    st->st_rdev     = 0;
  #endif
    st->st_native_mode = atr.dwFileAttributes;
    st->st_mode     = FKS_W32FATTER_TO_STATMODE( atr.dwFileAttributes );
    st->st_size     = ((uint64_t)atr.nFileSizeHigh << 32) | atr.nFileSizeLow;
    st->st_ctime    = FKS_W32FTIME_TO_TIME( FKS_U32X2P_TO_U64( &atr.ftCreationTime   ) );
    st->st_atime    = FKS_W32FTIME_TO_TIME( FKS_U32X2P_TO_U64( &atr.ftLastAccessTime ) );
    st->st_mtime    = FKS_W32FTIME_TO_TIME( FKS_U32X2P_TO_U64( &atr.ftLastWriteTime  ) );
    st->st_ex_mode  = (rc <= 0) ? FKS_S_EX_ERROR : 0;
    return (rc > 0) - 1;
}

#if 0
/** set file time
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_futimes(fks_fh_t h, struct fks_timeval* tv) FKS_NOEXCEPT
{
    typedef const FILETIME* CFT;
    int64_t lastAcs, lastWrt;
    FKS_ARG_ASSERT(1, h != ((intptr_t)-1));
    FKS_ARG_PTR0_ASSERT(1, tv);
    if (tv == 0)
        return -1;
    lastAcs = (tv->tv_sec * (int64_t)1000 * 1000 + tv->tv_usec) * (int64_t)1000;
    ++tv;
    lastWrt = (tv->tv_sec * (int64_t)1000 * 1000 + tv->tv_usec) * (int64_t)1000;
    return SetFileTime(FKS_PRIV_FH2WH(h), NULL, (CFT)&lastAcs, (CFT)&lastWrt) ? 0/*OK*/ : -1/*NG*/;
}
#endif

/** Obtaining time 0 if successful, returning negative if fainted.
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_fhGetTime(fks_fh_t h, fks_time_t* pCreat, fks_time_t* pLastAcs, fks_time_t* pLastWrt) FKS_NOEXCEPT
{
    typedef FILETIME* FT;
    return GetFileTime(FKS_PRIV_FH2WH(h), (FT)pCreat, (FT)pLastAcs, (FT)pLastWrt) ? 0 : -1;
}


/** Time setting.The value is system dependent.
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_fhSetTime(fks_fh_t fh, fks_time_t crt, fks_time_t acs, fks_time_t wrt) FKS_NOEXCEPT
{
    typedef FILETIME* FT;
    return SetFileTime((HANDLE)fh, (FT)&crt, (FT)&acs, (FT)&wrt) ? 0 : -1;
}


/** Get file time (system value)
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_fileGetTime(const char* fname, fks_time_t* pCr, fks_time_t* pAcs, fks_time_t* pWrt) FKS_NOEXCEPT
{
    WIN32_FILE_ATTRIBUTE_DATA    atr = {0};
    int rc;
 #ifdef FKS_USE_LONGFNAME
    {
        wchar_t* fnameW;
        FKS_LONGFNAME_FROM_CS_INI(1);
        FKS_LONGFNAME_FROM_CS(0, fnameW, fname);
        rc = GetFileAttributesExW(fnameW, GetFileExInfoStandard, &atr);
    }
 #else
    rc = GetFileAttributesExA(fname, GetFileExInfoStandard, &atr);
 #endif
    if (pCr ) *pCr  = FKS_U32X2P_TO_U64(&atr.ftCreationTime  );
    if (pAcs) *pAcs = FKS_U32X2P_TO_U64(&atr.ftLastAccessTime);
    if (pWrt) *pWrt = FKS_U32X2P_TO_U64(&atr.ftLastWriteTime );
    return (rc > 0) - 1;
}


/** Is directory?
 */
FKS_LIB_DECL (int)
fks_isDir(const char* fname) FKS_NOEXCEPT
{
    uint32_t m;
    FKS_ARG_PTR_ASSERT(1, fname);
    m = fks_fileAttr(fname);
    return (m != 0xFFFFFFFF) && (m & FILE_ATTRIBUTE_DIRECTORY);
}


/** Get file attributes.
 */
FKS_LIB_DECL (unsigned int)
fks_fileAttr(const char* fpath) FKS_NOEXCEPT
{
    FKS_ARG_PTR_ASSERT(1, fpath);
 #ifdef FKS_USE_LONGFNAME
    {
        wchar_t* fpathW;
        FKS_LONGFNAME_FROM_CS_INI(1);
        FKS_LONGFNAME_FROM_CS(0, fpathW, fpath);
        return GetFileAttributesW(fpathW);
    }
 #else
    return GetFileAttributesA(fpath);
 #endif
}


/** Get file size
 */
FKS_LIB_DECL (fks_off64_t)
fks_fileSize(const char* fname) FKS_NOEXCEPT
{
    HANDLE           h;
 #ifdef FKS_USE_LONGFNAME
    if (fname) {
        WIN32_FIND_DATAW d;
        wchar_t* fnameW;
        FKS_LONGFNAME_FROM_CS_INI(1);
        FKS_LONGFNAME_FROM_CS(0, fnameW, fname);
        h = FindFirstFileW(fnameW, &d);
        if (h != (HANDLE)INVALID_HANDLE_VALUE) {
            FindClose(h);
            return (((uint64_t)d.nFileSizeHigh<<32) | d.nFileSizeLow);
        }
    }
    return 0;
 #else
    WIN32_FIND_DATAA d;
    return fname
             && ((h = FindFirstFileA(fname,&d)) != (HANDLE)INVALID_HANDLE_VALUE)
             && FindClose(h)
           ? (((uint64_t)d.nFileSizeHigh<<32) | d.nFileSizeLow) : 0;
 #endif
}


/** Exist file ?
 */
FKS_LIB_DECL (int)
fks_fileExist(const char* fname) FKS_NOEXCEPT
{
    FKS_ARG_PTR_ASSERT(1, fname);
 #if 1
    return 0xFFFFFFFF != fks_fileAttr(fname);
 #else
  #ifdef FKS_USE_LONGFNAME
    {
        wchar_t* fpathW;
        FKS_LONGFNAME_FROM_CS_INI(1);
        FKS_LONGFNAME_FROM_CS(0, fpathW, fpath);
        return 0xFFFFFFFF != GetFileAttributesW(fnameW);
    }
  #else
    return 0xFFFFFFFF != GetFileAttributesA(fname);
  #endif
 #endif
}


/** rename old file to new file
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_fileMove(const char* srcname, const char* dstname, int overwriteFlag) FKS_NOEXCEPT
{
    unsigned flags = MOVEFILE_COPY_ALLOWED | (overwriteFlag ? MOVEFILE_REPLACE_EXISTING : 0);
 #ifdef FKS_USE_LONGFNAME
    wchar_t* srcnameW;
    wchar_t* dstnameW;
    FKS_LONGFNAME_FROM_CS_INI(2);
    FKS_ARG_PTR_ASSERT(1, srcname);
    FKS_ARG_PTR_ASSERT(2, dstname);
    FKS_LONGFNAME_FROM_CS(0, srcnameW, srcname);
    FKS_LONGFNAME_FROM_CS(1, dstnameW, dstname);
    return MoveFileExW( srcnameW, dstnameW, flags ) ? 0 : -1;
 #else
    FKS_ARG_PTR_ASSERT(1, srcname);
    FKS_ARG_PTR_ASSERT(2, dstname);
    return MoveFileExA( srcname, dstname, flags ) ? 0 : -1;
 #endif
}


/** copy src file to dst file
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_fileCopy(const char* srcname, const char* dstname, int overwriteFlag) FKS_NOEXCEPT
{
    unsigned flags = MOVEFILE_COPY_ALLOWED | (overwriteFlag ? MOVEFILE_REPLACE_EXISTING : 0);
 #ifdef FKS_USE_LONGFNAME
    wchar_t* srcnameW;
    wchar_t* dstnameW;
    FKS_LONGFNAME_FROM_CS_INI(2);
    FKS_ARG_PTR_ASSERT(1, srcname);
    FKS_ARG_PTR_ASSERT(2, dstname);
    FKS_LONGFNAME_FROM_CS(0, srcnameW, srcname);
    FKS_LONGFNAME_FROM_CS(1, dstnameW, dstname);
    //return CopyFileExW( srcnameW, dstnameW, NULL, NULL, NULL, flags ) ? 0 : -1;
    return CopyFileW( srcnameW, dstnameW, flags ) ? 0 : -1;
 #else
    FKS_ARG_PTR_ASSERT(1, srcname);
    FKS_ARG_PTR_ASSERT(2, dstname);
    return CopyFileA( srcname, dstname, flags ) ? 0 : -1;
 #endif
}


#if 1
/** 時間の設定.     値はシステム依存.
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_fileSetTime(const char* fname, fks_time_t crt, fks_time_t acs, fks_time_t wrt) FKS_NOEXCEPT
{
 #if 1
    fks_fh_t fh = fks_open(fname, FKS_O_RDWR|FKS_O_BINARY, 0777);
    int      rc = 0;
    if (fh) {
        rc = fks_fhSetTime(fh, crt, acs, wrt);
        fks_close(fh);
    }
    return rc;
 #else
    int    rc  = 0;
    HANDLE hdl = CreateFileA(fname, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
    if (hdl != INVALID_HANDLE_VALUE) {
        rc = SetFileTime(hdl, (FILETIME*)&crt, (FILETIME*)&acs, (FILETIME*)&wrt);
        CloseHandle(hdl);
    }
    return rc ? 0 : -1;
 #endif
}
#endif


//===========================================================================
#if 1
FKS_LIB_DECL (fks_fh_t)     fks_fh_in(void) FKS_NOEXCEPT
{
    return (fks_fh_t)GetStdHandle(STD_INPUT_HANDLE );
}

FKS_LIB_DECL (fks_fh_t)     fks_fh_out(void) FKS_NOEXCEPT
{
    return (fks_fh_t)GetStdHandle(STD_OUTPUT_HANDLE);
}

FKS_LIB_DECL (fks_fh_t)     fks_fh_err(void) FKS_NOEXCEPT
{
    return (fks_fh_t)GetStdHandle(STD_ERROR_HANDLE );
}

FKS_FAST_DECL (fks_fh_t)
fks_priv_fh_conv(fks_fh_t hdl) FKS_NOEXCEPT
{
    //FKS_ARG_ASSERT(1, hdl != ((fks_fh_t)-1));
    if (hdl <= (fks_fh_t)2) {
        if (hdl == (fks_fh_t)1)     hdl = fks_fh_out();
        else if (hdl < (fks_fh_t)1) hdl = fks_fh_in();
        else                        hdl = fks_fh_err();
    }
    return hdl;
}
#endif


//===========================================================================
#if 1
/** ファイル名のフルパス化. (実際のカレントディレクトリを反映)
 */
FKS_LIB_DECL (char*)
fks_fileFullpath(char fpath[], size_t l, const char* src) FKS_NOEXCEPT
{
    int rc;
    FKS_ARG_PTR_ASSERT(1, fpath);
    FKS_ARG_ASSERT(2, l > 0);
    FKS_ARG_PTR_ASSERT(3, src);
 #ifdef FKS_USE_LONGFNAME
    {
        wchar_t* srcW;
        FKS_LONGFNAME_FROM_CS_INI(1);
        wchar_t* fpathW = (wchar_t*)fks_alloca((l+6)*sizeof(wchar_t));
        FKS_LONGFNAME_FROM_CS(0, srcW, src);
        rc = GetFullPathNameW(srcW, l, fpathW, NULL);
        if (rc > 0) {
            rc = FKS_MBS_FROM_WCS(0, 0, fpathW, rc+1);
            if ((size_t)rc >= l)
                return NULL;
            rc = FKS_MBS_FROM_WCS(fpath, l, fpathW, rc+1);
        }
    }
 #else
    rc = GetFullPathNameA(src, l, fpath, NULL);
 #endif
    return (rc > 0) ? fpath : NULL;
}
#endif


//===========================================================================
#if 1
//@@@ 要W化.
/** exeのパス名取得.
 *  @return 0:ok  負:エラー. 詳細はfks_lastError()から取得.
 */
FKS_LIB_DECL (char*)
fks_getExePath(char nameBuf[], size_t nameBufSize) FKS_NOEXCEPT
{
    return GetModuleFileNameA(0, nameBuf, nameBufSize) ? nameBuf : 0;
}


/** システムディレクトリの取得.
 */
FKS_LIB_DECL (char*)
fks_getSystemDir(char nameBuf[], size_t nameBufSize) FKS_NOEXCEPT
{
    return GetSystemDirectoryA(nameBuf, nameBufSize) ? nameBuf: 0;
}


/** windowsディレクトリの取得.
 */
FKS_LIB_DECL (char*)
fks_getWindowsDir(char nameBuf[], size_t nameBufSize) FKS_NOEXCEPT
{
    return GetWindowsDirectoryA(nameBuf, nameBufSize) ? nameBuf: 0;
}
#endif


//===========================================================================
#if 1

#define MKDIR_PMODE 0777


FKS_STATIC_DECL (int)
fks_recursiveMkDir_subr(const char* name);

/** mkdir 拡張. 途中のディレクトリも作成する.
 *  Extension of mkdir, making directories on the way
 */
FKS_LIB_DECL (fks_io_rc_t)
fks_recursiveMkDir(const char *name, int dummy_pmode) FKS_NOEXCEPT
{
    uint32_t        atr;
    atr = fks_fileAttr(name);
    if (atr != (uint32_t)-1) {
        if (atr & FKS_FILE_ATTR_DIRECTORY)
            return 0;   // Directory : OK.
        return -1;      // File : NG.
    }

    if (fks_mkdir(name, MKDIR_PMODE) == 0)
        return 0;   // ok.

    return fks_recursiveMkDir_subr(name);
}

/** ※ 通常のmkdirでのスタッフ消費を抑えるため、別関数にしている.
 *  *  In order to suppress the consumption of stuff in ordinary mkdir, we have made another function.
 */
FKS_STATIC_DECL (int)
fks_recursiveMkDir_subr(const char* name)
{
    char    nm[ FKS_PATH_MAX_URL + 1 ];
    char*   e;
    char*   s;

    fks_pathCpy(nm, FKS_PATH_MAX_URL, name);
    e = nm + fks_pathLen(nm);
    do {
        s = fks_pathBaseName(nm);
        if (s <= nm)
            return -1;  // 失敗.
        --s;
        *s = 0;
    } while (fks_mkdir(nm, MKDIR_PMODE) != 0);
    do {
        *s    = FKS_PATH_SEP_CHR;
        s    += fks_pathLen(s);
        if (s >= e)
            return fks_mkdir(nm, MKDIR_PMODE);
    } while (fks_mkdir(nm, MKDIR_PMODE) == 0);
    return -1;
}

#endif



//===========================================================================
#if 1


/** 環境変数 tmp または temp を tmpEnvに入れる. 無ければ "."を入れる.
 *  If there is an environment variable tmp or temp, it returns its contents, if not, it returns "."
 *  @return     0: none  1:There was tmp, temp
 */
FKS_LIB_DECL (int)
fks_getTmpEnv(char tmpEnv[], size_t size) FKS_NOEXCEPT
{
    const char* p = getenv("TMP");
    int         f = 1;
    FKS_ARG_PTR_ASSERT(1, tmpEnv);
    FKS_ARG_ASSERT(2, size > 0);
    if (p == NULL) {
        p = getenv("TEMP");
        if (p == NULL) {
            p = ".";
            f = 0;
        }
    }
    fks_pathCpy(tmpEnv, size, p);
    return f;
}


/** テンポラリファイル名作成. 成功するとnameを返し、失敗だとNULL.
 *  prefix,surffix でファイル名の両端文字列を設定.
 *  成功すると、テンポラリディレクトリにその名前のファイルができる.(close済)
 *  (つまり自分で削除しないと駄目)
 */
FKS_LIB_DECL (char*)
fks_tmpFile(char path[], size_t size, const char* prefix, char const* suffix) FKS_NOEXCEPT
{
  #ifdef _WIN32
    HANDLE      h;
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
        if (fks_isDir(path))
            fks_pathCpy(tmpd, FKS_PATH_MAX, path);
    }
    if (tmpd[0] == 0)
        fks_getTmpEnv(tmpd, FKS_PATH_MAX);
    //printf("dir=%s\n", tmpd);
    pid = GetCurrentProcessId();
    pid = ((pid / 41) * 17 + (pid % 41)*0x10003) ^ ( 0x00102101);
    QueryPerformanceCounter((union _LARGE_INTEGER*)&tmr);   // time(&tmr);
    tmr *= 16;
    idx = 0;
    do {
        unsigned ti;
        ++idx;
        ti = (unsigned)(tmr + idx);
        snprintf(path, size-1, "%s\\%s%08x-%08x%s", tmpd, prefix, pid, ti, suffix);
        path[size-1] = 0;
        h = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL); //@@@ TODO: CreateFileW
    } while (h == INVALID_HANDLE_VALUE && idx > 0);
    if (h == INVALID_HANDLE_VALUE)
        return NULL;
    CloseHandle(h);
    return path;
  #endif
}

#endif

//===========================================================================

#if 0
FKS_LIB_DECL (int)
fks_fileDateCmp(const char *lhs, const char *rhs)
{
    fks_time_t  lt  = 0, rt = 0;
    fks_io_rc_t lrc, rrc;
    FKS_ARG_PTR_ASSERT(1, lhs);
    FKS_ARG_PTR_ASSERT(2, rhs);
    lrc = fks_fileGetTime(lhs, NULL, &lt, NULL);
    rrc = fks_fileGetTime(rhs, NULL, &rt, NULL);

    if (lrc >= 0 && rrc >= 0)
        return (lt < rt) ? -1 : (lt > rt) ? 1 : 0;
    if (lrc < 0 && rrc < 0)
        return 0;
    return (lrc < 0) ? -1 : 1;
}
#endif


#ifdef __cplusplus
}
#endif

#endif  // FKS_WIN32
