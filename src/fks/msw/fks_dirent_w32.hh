/*
 *  @file   fks_dirent_w32.hh
 *  @brief
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks_dirent.h>
#include <fks_malloc.h>
#include <fks_assert_ex.h>
#include <fks_path.h>
#include <string.h>
#include <stdio.h>

#include <windows.h>
#include <shlwapi.h>
#if _MSC_VER
#pragma comment(lib, "shlwapi.lib")
#endif

#include <msw/fks_io_priv_w32.h>

#undef WIN32_FIND_DATA
#undef FindNextFile
#ifdef FKS_USE_LONGFNAME
#define WIN32_FIND_DATA     WIN32_FIND_DATAW
#define FindNextFile        FindNextFileW
#else
#define WIN32_FIND_DATA     WIN32_FIND_DATAA
#define FindNextFile        FindNextFileA
#endif

typedef struct Fks_DirEntFindData {
    Fks_DirEnt  dirent;
    fks_stat_t  stat;
    char        path[FKS_PATH_MAX * 6 + 16];
} Fks_DirEntFindData;


FKS_INLINE_DECL (size_t)
fks_getDirEntFromWin32(Fks_DirEnt* d, WIN32_FIND_DATA const* s, char* name, size_t nameSz)
{
    fks_stat_t* st      = d->stat;
    size_t      l       = 0;
    FKS_ARG_PTR_ASSERT(1, d);
    FKS_ARG_PTR_ASSERT(2, s);
    FKS_ARG_PTR_ASSERT(3, name);
    FKS_ARG_ASSERT(4, nameSz > 0);
    st->st_size         = ((int64_t)s->nFileSizeHigh << 32) | s->nFileSizeLow;
    st->st_ctime        = FKS_W32FTIME_TO_TIME( FKS_U32X2P_TO_U64( &s->ftCreationTime ) );
    st->st_atime        = FKS_W32FTIME_TO_TIME( FKS_U32X2P_TO_U64( &s->ftLastAccessTime ) );
    st->st_mtime        = FKS_W32FTIME_TO_TIME( FKS_U32X2P_TO_U64( &s->ftLastWriteTime  ) );
    st->st_mode         = FKS_W32FATTER_TO_STATMODE( s->dwFileAttributes );
    st->st_native_attr  = s->dwFileAttributes;

    d->name = name;
 #ifdef FKS_USE_LONGFNAME
    l       = FKS_MBS_FROM_WCS(NULL, 0, s->cFileName, wcslen(s->cFileName)) + 1;
    if (l > nameSz)
        l  = nameSz;
    FKS_MBS_FROM_WCS(name, nameSz, s->cFileName, wcslen(s->cFileName)+1);
 #else
    l       = strlen(s->cFileName) + 1;
    if (l > nameSz)
        l = nameSz;
    fks_pathCpy(name, l, s->cFileName);
 #endif
    d->sub  = NULL;
    return l;
}


FKS_STATIC_DECL(void)
Fks_DirEntFindData_init(Fks_DirEntFindData* wk, WIN32_FIND_DATA const* findData)
{
    memset(wk, 0, sizeof *wk);
    wk->dirent.stat = &wk->stat;
    fks_getDirEntFromWin32(&wk->dirent, findData, wk->path, sizeof wk->path);
}


FKS_LIB_DECL (Fks_DirEntries*)
fks_getDirEntries1(Fks_DirEntries* dirEntries, char const* dirPath, char const* pattern, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
    typedef struct LinkData {
        struct LinkData*    link;
        Fks_DirEntFindData  data;
    } LinkData;
    HANDLE              hdl;
    size_t              entSz;
    size_t              statSz;
    size_t              strSz;
    size_t              n;
    size_t              l;
    char*               buf;
    char*               strp;
    char*               strp_end;
    fks_stat_t*         statp;
    Fks_DirEnt*         d;
    LinkData*           t;
    LinkData            root = { 0 };
    WIN32_FIND_DATA     findData = { 0 };
    Fks_DirEntFindData  deFindData;
    char const*         srchPath = NULL;
 #ifdef FKS_USE_LONGFNAME
    wchar_t*            pathW = NULL;
    FKS_LONGFNAME_FROM_CS_INI(1);
 #endif
    FKS_ARG_PTR_ASSERT(1, dirEntries);
    FKS_ARG_PTR_ASSERT(2, dirPath);
    FKS_ARG_PTR_ASSERT(3, pattern);
    if (!dirEntries)
        return NULL;
    memset(dirEntries,0, sizeof *dirEntries);
    if (!dirPath && !pattern) {
        FKS_ASSERT(dirPath || pattern);
        return NULL;
    }
    if (pattern == NULL || pattern[0] == 0)
        pattern = "*";

    l        = strlen(dirPath) + 1 + 1/*strlen("*")*/ + 1;
    srchPath = (char*)fks_alloca(l);
    fks_pathJoin((char*)srchPath, l, dirPath, "*");

 #ifdef FKS_USE_LONGFNAME
    FKS_LONGFNAME_FROM_CS(0, pathW, srchPath);
    hdl = FindFirstFileW(pathW, &findData);
 #else
    hdl = FindFirstFileA(srchPath, &findData);
 #endif
    if (hdl == INVALID_HANDLE_VALUE)
        return NULL;
    l = strlen(dirPath) + 1;
    strSz = l;

    // pool WIN32_FIND_DATA data
    t    = &root;
    n    = 0;
    do {
        Fks_DirEntFindData_init(&deFindData, &findData);
        if (!(flags & FKS_DE_DotAndDotDot) && (!strcmp(deFindData.path, ".") || !strcmp(deFindData.path, ".."))) {
            continue;
        }
        if ((flags & FKS_DE_DirOnly) && !(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;
        if ((flags & FKS_DE_FileOnly) && (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;
        if (!(flags & FKS_DE_Recursive) || !(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (isMatch && isMatch(&deFindData.dirent) == 0)
                continue;
            if (fks_pathMatchWildCard(pattern, deFindData.path) == 0)
                continue;
        } else {    // recursive directory
            if (fks_pathMatchWildCard(pattern, deFindData.path) == 0)
                deFindData.stat.st_ex_mode |= FKS_S_EX_NOTMATCH;
        }
        t->link = (LinkData*)fks_calloc(1, sizeof(LinkData));
        if (t == NULL) {
            dirEntries = NULL;
            FindClose(hdl);
            goto ERR;
        }
        t       = t->link;
        t->data = deFindData;
        strSz  += strlen(deFindData.path) + 1;
        //printf("%p : %p %s\n", t, t->link, t->data.cFileName);
        ++n;
    } while (FindNextFile(hdl, &findData) != 0);
    FindClose(hdl);

    //
    entSz  = (n + 1) * sizeof(Fks_DirEnt);
    statSz = n * sizeof(fks_stat_t);
    buf    = (char*)fks_calloc(1, entSz + statSz + strSz + 1);
    if (buf == NULL) {
        dirEntries = NULL;
        goto ERR;
    }
    d                   = (Fks_DirEnt*)buf;
    dirEntries->entries = d;
    statp               = (fks_stat_t*)(buf + entSz);
    strp                = buf + entSz + statSz;
    strp_end            = strp + strSz;
    l = strlen(dirPath) + 1;
    memcpy(strp, dirPath, l);
    dirEntries->path    = strp;
    strp += l;
    for (t = root.link; t; t = t->link) {
        *statp  = t->data.stat;
        d->stat = statp++;
        l = strlen(t->data.path) + 1;
        memcpy(strp, t->data.path, l);
        d->name = strp;
        strp  += l;
        d->sub = NULL;
        // printf("%s\t%lld\n",d->name, d->stat->st_size);
        ++d;
    }
    dirEntries->size    = n;

 ERR:
    // remove work table
    t = root.link;
    while (t) {
        LinkData* u = t->link;
        fks_free(t);
        t = u;
    }
    return dirEntries;
}
