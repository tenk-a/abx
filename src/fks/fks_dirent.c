/*
 *  @file   fks_dirent.c
 *  @brief
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks_dirent.h>
#include <fks_malloc.h>
#include <fks_assert_ex.h>
#include <fks_alloca.h>
#include <fks_path.h>
#include <string.h>
#include <stdlib.h>

#ifdef FKS_WIN32
#include "msw/fks_dirent_w32.hh"
#elif defined FKS_LINUX // || defined FKS_BSD
#include "uni/fks_dirent_uni.hh"
#endif

#define FKS_S_W32_IS_DIR(a)		((a) & FKS_S_W32_Directory)
#define FKS_S_W32_IS_FILE(a)	((a) & FKS_S_W32_Directory)


int fks_dirEnt_isMatchStartWithNonDot(void* dmy, Fks_DirEnt const* ent)
{
    FKS_ARG_PTR_ASSERT(1, ent);
    return ent->name[0] && ent->name[0] != '.';
}


FKS_LIB_DECL (Fks_DirEntries*) fks_createDirEntries1a(Fks_DirEntries* dirEntries, char const** dirPath, char const* fname
                                                        , unsigned int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT;

FKS_STATIC_DECL (Fks_DirEntries*)
fks_createDirEntriesMT(Fks_DirEntries* dirEntries, char const* dirPath, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT;


FKS_LIB_DECL (Fks_DirEntries*)
fks_createDirEntries(Fks_DirEntries* dirEntries, char const* dirPath, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT
{
	Fks_DirEnt_Matchs	mtBuf;
	if (!mt) {
		memset(&mtBuf, 0, sizeof mtBuf);
		mt = &mtBuf;
	}
    if (mt->fname == NULL || mt->fname[0] == 0)
        mt->fname = "*";
    if (!dirPath) {
        size_t l = strlen(mt->fname) + 1;
        dirPath = (char*)fks_alloca(l);
        fks_pathGetDir((char*)dirPath, l, mt->fname);
        mt->fname = fks_pathBaseName(mt->fname);
    }
	return fks_createDirEntriesMT(dirEntries, dirPath, mt);
}

FKS_LIB_DECL (Fks_DirEntries*)
fks_createDirEntriesMT(Fks_DirEntries* dirEntries, char const* dirPath, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT
{
	unsigned flags;
    FKS_ARG_PTR_ASSERT(1, dirEntries);
    FKS_ARG_PTR_ASSERT(2, dirPath);
    FKS_ARG_PTR_ASSERT(3, mt);

	flags = mt->flags;
	if (flags == 0) {
		flags = FKS_DE_Dir | FKS_DE_File | FKS_DE_Hidden | FKS_DE_DotOrDotDot;
		mt->flags = flags;
	}
    dirEntries  = fks_getDirEntries1(dirEntries, dirPath, mt->fname, flags, mt->isMatch, mt->isMatchData);

    if ((flags & FKS_DE_Recursive) && dirEntries) {
        fks_isize_t i;
        size_t      l;
        fks_isize_t m           = 0;
        fks_isize_t n           = dirEntries->size;
        char*       path        = NULL;
        Fks_DirEnt* entries     = (Fks_DirEnt*)dirEntries->entries;
        for (i = 0; i < n; ++i) {
            Fks_DirEnt* d   = &entries[i];
            if (FKS_DE_IsDotOrDotDot(d->name))
                continue;
            if (FKS_S_ISDIR(d->stat->st_mode)) {
                Fks_DirEntries* des;
                if (mt->isDirMatch && mt->isDirMatch(mt->isDirMatchData, d) == 0)
                	continue;
                des = (Fks_DirEntries*)fks_calloc(1, sizeof(Fks_DirEntries));
                if (!des) {
                    //if (flags & FKS_DE_ErrCont)
                    //  continue;
                    fks_free(path);
                    return NULL;
                }
                l = strlen(dirPath) + 1 + strlen(d->name) + 1;
                if (l < FKS_PATH_MAX)
                    l = FKS_PATH_MAX;
                if (m < l) {
                    m    = l;
                    path = (char*)fks_realloc(path, l);
                    if (!path) {
                        return NULL;
                    }
                }
                fks_pathCombine(path, l, dirPath, d->name);
                if (fks_createDirEntries(des, path, mt)) {
                    d->sub = des;
                } else {
                    fks_free(des);
                }
            }
        }
        fks_free(path);
    }
    return dirEntries;
}

FKS_LIB_DECL (void)
fks_releaseDirEntries(Fks_DirEntries* dirEntries) FKS_NOEXCEPT
{
    fks_isize_t     i, n;
    Fks_DirEnt*     entries;
    if (!dirEntries)
        return;
    n       = dirEntries->size;
    entries = (Fks_DirEnt*)dirEntries->entries;
    for (i = 0; i < n; ++i) {
        Fks_DirEnt* d = &entries[i];
        if (d->sub)
            fks_releaseDirEntries(d->sub);
    }
    fks_free(entries);
    dirEntries->entries = NULL;
    dirEntries->path    = NULL;
    dirEntries->size    = 0;
}

FKS_STATIC_DECL (fks_isize_t) fks_foreachDirEntriesMT(Fks_DirEntries* dirEntries, Fks_ForeachDirEntCB cb
													, void* data, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT;

/// @return >=0:ok   -1:cb () returned 0   -2:error
///
FKS_LIB_DECL (fks_isize_t)
fks_foreachDirEntries(Fks_DirEntries* dirEntries, Fks_ForeachDirEntCB cb, void* data, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT
{
	Fks_DirEnt_Matchs	mtBuf;
	if (!mt) {
		memset(&mtBuf, 0, sizeof mtBuf);
		mt = &mtBuf;
	}
	return fks_foreachDirEntriesMT(dirEntries, cb, data, mt);
}

FKS_STATIC_DECL (fks_isize_t)
fks_foreachDirEntriesMT(Fks_DirEntries* dirEntries, Fks_ForeachDirEntCB cb, void* data, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT
{
    fks_isize_t         i, n, cnt;
    Fks_DirEnt const*   entries;
    char const*         dirPath;
    int                 curParFlag;
    unsigned int		flags;
    FKS_ARG_PTR_ASSERT(1, dirEntries);
    FKS_ARG_PTR_ASSERT(2, cb);
    FKS_ARG_PTR_ASSERT(3, mt);
    if (!dirEntries || !cb || !mt)
        return -2;
    cnt     = 0;
    n       = dirEntries->size;
    entries = dirEntries->entries;
    dirPath = dirEntries->path;
    flags	= mt->flags;
    for (i = 0; i < n; ++i) {
        Fks_DirEnt const* d = &entries[i];
        if (d->name == NULL || d->stat == NULL)
            continue;
        curParFlag = FKS_DE_IsDotOrDotDot(d->name);
        if (!(flags & FKS_DE_DotOrDotDot) && curParFlag)
            continue;
        if (FKS_DE_IsDirOnly(flags) && !FKS_S_ISDIR(d->stat->st_mode))
            continue;
        if (!(flags & FKS_DE_Recursive) && FKS_DE_IsFileOnly(flags) && FKS_S_ISDIR(d->stat->st_mode))
            goto NEXT;

	 #ifdef FKS_WIN32
		if (   (!(flags & FKS_DE_Hidden) && (d->stat->st_native_attr & FKS_S_W32_Hidden))
//		    || ((flags & FKS_DE_ReadOnly) && !(d->stat->st_native_attr & FKS_S_W32_ReadOnly))
		){
            goto NEXT;
		}
	 #endif

        if (mt->isMatch && mt->isMatch(mt->isMatchData, d) == 0)
            goto NEXT;
        if (cb(data, d, dirPath) == 0)
            return -1;  // foreach break
        ++cnt;
      NEXT:
        if (d->sub && !curParFlag) {
			fks_isize_t cnt2;
            if (mt->isDirMatch && mt->isDirMatch(mt->isDirMatchData, d) == 0)
            	continue;
            cnt2 = fks_foreachDirEntries(d->sub, cb, data, mt);
            if (cnt2 < 0)
                return cnt2;    // foreach break
            cnt += cnt2;
        }
    }
    return cnt;     // foreach continue
}


FKS_STATIC_DECL (fks_isize_t) fks_createDirEntPathStatSub(void** ppAry, char const* dirPath
								, unsigned int flags, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT;
FKS_STATIC_DECL (fks_isize_t) fks_convDirEntPathStatSub(void** ppAry, Fks_DirEntries* dirEntries
								, unsigned int flags, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT;

static int fks_countDirEntries_sub(void* pCnt, Fks_DirEnt const* dmy_ent, char const* dmy_dirName) FKS_NOEXCEPT
{
    ++*(size_t*)pCnt;
    return 1;
}

static int fks_countDirEntries_sub2(void* pCnt, Fks_DirEnt const* ent, char const* dirPath) FKS_NOEXCEPT
{
    ++((size_t*)pCnt)[0];
    ((size_t*)pCnt)[1] += strlen(dirPath) + 1 + strlen(ent->name) + 1;
    return 1;
}

FKS_LIB_DECL (fks_isize_t)
fks_countDirEntries(size_t* strBytes, Fks_DirEntries* dirEntries, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT
{
    size_t 				cnt[2] = {0};
	Fks_DirEnt_Matchs	mtBuf;
	if (!mt) {
		memset(&mtBuf, 0, sizeof mtBuf);
		mt = &mtBuf;
	}
    fks_foreachDirEntries(dirEntries, (strBytes) ? fks_countDirEntries_sub2 : fks_countDirEntries_sub, cnt, mt);
    if (strBytes)
        *strBytes = cnt[1];
    return (fks_isize_t)cnt[0];
}

FKS_LIB_DECL (fks_isize_t)
fks_convDirEntPathStats(Fks_DirEntPathStat** ppPathStats , Fks_DirEntries* dirEntries, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT
{
    return fks_convDirEntPathStatSub((void**)ppPathStats, dirEntries, FKS_DE_NameStat, mt);
}

FKS_LIB_DECL (fks_isize_t)
fks_convDirEntPaths(char*** pppPaths, Fks_DirEntries* dirEntries, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT
{
    return fks_convDirEntPathStatSub((void**)pppPaths, dirEntries, FKS_DE_Tiny, mt);
}

FKS_LIB_DECL (fks_isize_t)
fks_createDirEntPathStats(Fks_DirEntPathStat** ppPathStats , char const* dirPath, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT
{
    return fks_createDirEntPathStatSub((void**)ppPathStats, dirPath, FKS_DE_NameStat, mt);
}

FKS_LIB_DECL (fks_isize_t)
fks_createDirEntPaths(char*** pppPaths, char const* dirPath, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT
{
    return fks_createDirEntPathStatSub((void**)pppPaths, dirPath, FKS_DE_Tiny, mt);
}

FKS_STATIC_DECL (fks_isize_t)
fks_createDirEntPathStatSub(void** ppAry, char const* dirPath, unsigned int flags, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT
{
	Fks_DirEnt_Matchs	mt1;
    fks_isize_t     	n;
    Fks_DirEntries  	dirEntries;
	Fks_DirEnt_Matchs	mt2;
    FKS_ARG_PTR_ASSERT(1, ppAry);
    if (!ppAry)
        return -1;
    *ppAry = NULL;
    if (mt) {
		mt1 = *mt;
		mt  = &mt1;
	} else {
		memset(&mt1, 0, sizeof mt1);
		mt = &mt1;
	}
	mt->flags |= flags;
    if (mt->fname == NULL || mt->fname[0] == 0)
        mt->fname = "*";
    if (!dirPath) {
        size_t    l = strlen(mt->fname) + 1;
        dirPath 	= (char*)fks_alloca(l);
        fks_pathGetDir((char*)dirPath, l, mt->fname);
        mt->fname	= fks_pathBaseName(mt->fname);
		//printf("dirPath = %s  mt->fname = %s\n", dirPath, mt->fname);
    }
	mt2 = *mt;
	mt2.flags = mt->flags;
    if (fks_createDirEntriesMT(&dirEntries, dirPath, &mt2) == NULL)
        return -1;
    if (dirEntries.size <= 0)
        return dirEntries.size;
    n = fks_convDirEntPathStatSub(ppAry, &dirEntries, flags, &mt1);
    fks_releaseDirEntries(&dirEntries);
    return n;
}

FKS_LIB_DECL (void)
fks_releaseDirEntPathStats(Fks_DirEntPathStat* nameStats) FKS_NOEXCEPT
{
    fks_free(nameStats);
}


FKS_LIB_DECL (void)
fks_releaseDirEntPaths(char** names) FKS_NOEXCEPT
{
    fks_free(names);
}


typedef union Fks_DirEntNameStat_Uni {
    char**              name;
    Fks_DirEntPathStat* nameStat;
} Fks_DirEntNameStat_Uni;

typedef struct Fks_DirEntNameStat_Cur {
    Fks_DirEntNameStat_Uni  a;
    fks_stat_t*             stat;
    char*                   strs;
    char const*             pattern;
    unsigned int            flags;
    size_t                  count;
} Fks_DirEntNameStat_Cur;

static int fks_getDirEntNameStat_sub1(void* cur0, Fks_DirEnt const* d, char const* dirPath) FKS_NOEXCEPT;

FKS_STATIC_DECL (fks_isize_t)
fks_convDirEntPathStatSub(void** ppAry, Fks_DirEntries* dirEntries, unsigned int flags, Fks_DirEnt_Matchs* mt) FKS_NOEXCEPT
{
    fks_isize_t             n;
    fks_isize_t             cnt;
    size_t                  strSz;
    size_t                  nmSz;
    size_t                  statSz;
    char*                   buf;
    Fks_DirEntNameStat_Cur  cur;
	Fks_DirEnt_Matchs		mt1;
    FKS_ARG_PTR_ASSERT(1, ppAry);
    if (!ppAry)
        return -1;
    *ppAry = NULL;
    if (dirEntries->size <= 0)
        return dirEntries->size;
    if (mt) {
		mt1 = *mt;
		mt  = &mt1;
	} else {
		memset(&mt1, 0, sizeof mt1);
		mt = &mt1;
	}
	flags |= mt->flags;
	mt->flags = flags;
    n = fks_countDirEntries(&strSz, dirEntries, mt);
    if (n <= 0)
        return n;
    if (flags & FKS_DE_NameStat) {
        nmSz     = ((size_t)n + 1) * sizeof(Fks_DirEntPathStat);
        statSz   = (size_t)n * sizeof(fks_stat_t);
    } else {
        nmSz     = ((size_t)n + 1) * sizeof(char*);
        statSz   = 0;
        cur.stat = 0;
    }
    buf   = (char*)fks_calloc(1, nmSz + statSz + strSz);
    if (!buf)
        return -1;
    if (flags & FKS_DE_NameStat) {
        cur.a.nameStat  = (Fks_DirEntPathStat*)buf;
        cur.stat        = (fks_stat_t*)(buf + nmSz);
        cur.strs        = buf + nmSz + statSz;
    } else {
        cur.a.name      = (char**)buf;
        cur.stat        = NULL;
        cur.strs        = buf + nmSz;
    }
    cur.count   = 0;
    cur.flags   = flags;
    cur.pattern = mt->fname;
    cnt = fks_foreachDirEntriesMT(dirEntries, fks_getDirEntNameStat_sub1, &cur, mt);
    if (cnt < 0) {
        //if (flags & FKS_DE_ErrCont)
        //  *ppAry    = buf;
        //else
            fks_free(cur.a.name);
        return cnt;
    }
    *ppAry  = buf;
    FKS_ASSERT(cur.count <= n);
    return cur.count;
}

static int fks_getDirEntNameStat_sub1(void* cur0, Fks_DirEnt const* d, char const* dirPath) FKS_NOEXCEPT
{
    Fks_DirEntNameStat_Cur* c = (Fks_DirEntNameStat_Cur*)cur0;
    size_t                  l = strlen(dirPath) + 1 + strlen(d->name) + 1;

    if (d->stat->st_ex_mode & FKS_S_EX_NOTMATCH)
        return 1;

    fks_pathCombine(c->strs, l, dirPath, d->name);
    if (c->stat) {
        c->a.nameStat->path = c->strs;
        c->a.nameStat->stat = c->stat;
        *c->stat            = *d->stat;
        ++c->stat;
        ++c->a.nameStat;
    } else {
        *c->a.name = c->strs;
        ++c->a.name;
    }
    c->strs += l;
    ++c->count;
    return 1;
}
