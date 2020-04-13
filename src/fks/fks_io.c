/*
 *  @file   fks_io.c
 *  @brief  System level I/O functions and types.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks/fks_common.h>
#include <fks/fks_malloc.h>

#ifdef FKS_WIN32
#include "msw/fks_io_w32.hh"
#elif 1 //defined FKS_LINUX
#include "uni/fks_io_uni.hh"
#endif

#ifdef __cplusplus
extern "C" {
#endif

//===========================================================================
#if 1
/** file load
 */
FKS_LIB_DECL (void*)
fks_fileLoad(const char* fname, void* mem, size_t size, size_t* pReadSize) FKS_NOEXCEPT
{
    fks_fh_t    fh;
    FKS_ARG_PTR_ASSERT(1, fname);
    FKS_ARG_PTR_ASSERT(2, mem);
    FKS_ARG_ASSERT(3, size > 0);
    FKS_ARG_PTR0_ASSERT(4, pReadSize);
    fh = fks_open(fname, FKS_O_RDONLY|FKS_O_BINARY, 0);
    if (fh >= 0) {
        size_t rdSize = (size_t) fks_read(fh, mem, size);
        if (pReadSize)
            *pReadSize = rdSize;
        fks_close(fh);
    } else {
        mem = NULL;
    }
    return mem;
}
#endif

/** malloc & file load. (add EOS u32'\0')
 */
FKS_LIB_DECL (void*)
fks_fileLoadMalloc(const char* fname, size_t* pReadSize) FKS_NOEXCEPT
{
    fks_off64_t len   = fks_fileSize(fname);
    size_t      bytes = 0;
    char*       m;
    if (len == 0)
        return NULL;
    m = (char*)fks_calloc(1, len + 4);
    if (fks_fileLoad(fname, m, len, &bytes)) {
        if (bytes == len)
            return m;
    }
    fks_free(m);
    return NULL;
}

/** file save
 */
#if 1
FKS_LIB_DECL (void const*)
fks_fileSave(const char* fname, const void* mem, size_t size, int pmode) FKS_NOEXCEPT
{
    fks_fh_t    fh;
    size_t  n;
    FKS_ARG_PTR_ASSERT(1, fname);
    FKS_ARG_PTR_ASSERT(2, mem);
    FKS_ARG_ASSERT(3, size > 0);
    if (mem == 0 || size == 0)
        return 0;
    fh = fks_open(fname, FKS_O_CREAT|FKS_O_BINARY, pmode);
    if (fh < 0)
        return 0;
    n =(size_t) fks_write(fh, mem, size);
    fks_close(fh);
    return n == size ? mem : NULL;
}
#endif


#ifdef __cplusplus
}
#endif
