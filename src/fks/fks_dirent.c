/*
 *  @file   fks_dirent.c
 *  @brief
 *	@author	Masashi Kitamura (tenka@6809.net)
 *	@license Boost Software Lisence Version 1.0
 */

#include <fks/fks_dirent.h>
#include <fks/fks_malloc.h>
#include <fks/fks_assert_ex.h>
#include <fks/fks_alloca.h>
#include <string.h>
#include <stdlib.h>

#ifdef FKS_WIN32
#include "msw/fks_dirent_w32.cc"
#elif defined FKS_LINUX // || defined FKS_BSD
#include "linux/fks_dirent_linux.cc"
#endif


int fks_dirEnt_isMatchStartWithNonDot(Fks_DirEnt const* ent)
{
	FKS_ARG_PTR_ASSERT(1, ent);
	return ent->name[0] && ent->name[0] != '.';
}


FKS_LIB_DECL (Fks_DirEntries*)
fks_createDirEntries(Fks_DirEntries* dirEntries, char const* dirPath, char const* fname, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	if (fname == NULL || fname[0] == 0)
		fname = "*";
	if (!dirPath) {
		size_t l = strlen(fname) + 1;
		dirPath  = (char*)fks_alloca(l);
		fks_pathGetDir((char*)dirPath, l, fname);
		fname    = fks_pathBaseName(fname);
	}
	dirEntries = fks_getDirEntries1(dirEntries, dirPath, fname, flags, isMatch);
	if ((flags & FKS_DE_Recursive) && dirEntries) {
		fks_isize_t	i;
		size_t		l;
		fks_isize_t	m			= 0;
		fks_isize_t	n  			= dirEntries->size;
		char*		path		= NULL;
		Fks_DirEnt* entries		= (Fks_DirEnt*)dirEntries->entries;
		for (i = 0; i < n; ++i) {
			Fks_DirEnt* d = &entries[i];
			if (!strcmp(d->name, ".") || !strcmp(d->name, ".."))
				continue;
			if (FKS_S_ISDIR(d->stat->st_mode)) {
				Fks_DirEntries* des;
				if (flags & FKS_DE_FileOnly)
					continue;
				des = (Fks_DirEntries*)fks_calloc(1, sizeof(Fks_DirEntries));
				if (!des) {
					//if (flags & FKS_DE_ErrCont)
					//	continue;
					fks_free(path);
					return NULL;
				}
				l = strlen(dirPath) + 1 + strlen(d->name) + 1;
				if (l < FKS_PATH_MAX)
					l = FKS_PATH_MAX;
				if (m < l) {
					m    = l;
					path = (char*)fks_realloc(path, l);
				}
				fks_pathJoin(path, l, dirPath, d->name);
				if (fks_createDirEntries(des, path, fname, flags, isMatch)) {
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
	fks_isize_t		i, n;
	Fks_DirEnt*		entries;
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
	dirEntries->entries	= NULL;
	dirEntries->path	= NULL;
	dirEntries->size	= 0;
}

/// @return >=0:ok   -1:cb () returned 0   -2:error
///
FKS_LIB_DECL (fks_isize_t)
fks_foreachDirEntries(Fks_DirEntries* dirEntries, Fks_ForeachDirEntCB cb, void* data, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	fks_isize_t			i, n, cnt;
	Fks_DirEnt const*	entries;
	char const*			dirPath;
	FKS_ARG_PTR_ASSERT(1, dirEntries);
	FKS_ARG_PTR_ASSERT(1, cb);
	if (!dirEntries || !cb)
		return -2;
	cnt		= 0;
	n       = dirEntries->size;
	entries = dirEntries->entries;
	dirPath = dirEntries->path;
	for (i = 0; i < n; ++i) {
		Fks_DirEnt const* d = &entries[i];
		if (d->name == NULL || d->stat == NULL)
			continue;
		if (!(flags & FKS_DE_DotAndDotDot) && (!strcmp(d->name, ".") || !strcmp(d->name, "..")))
			continue;
		if ((flags & FKS_DE_DirOnly) && !FKS_S_ISDIR(d->stat->st_mode))
			continue;
		if ((flags & FKS_DE_FileOnly) && FKS_S_ISDIR(d->stat->st_mode))
			continue;
		if (isMatch && isMatch(d) == 0)
			continue;
		if (cb(data, d, dirPath) == 0)
			return -1;	// foreach break
		++cnt;
		if (d->sub) {
			fks_isize_t cnt2 = fks_foreachDirEntries(d->sub, cb, data, flags, isMatch);
			if (cnt2 < 0)
				return cnt2;	// foreach break
			cnt += cnt2;
		}
	}
	return cnt; 	// foreach continue
}


static int fks_countDirEntries_sub(void* pCnt, Fks_DirEnt const* dmy_ent, char const* dmy_dirName) FKS_NOEXCEPT {
	++*(size_t*)pCnt;
	return 1;
}
static int fks_countDirEntries_sub2(void* pCnt, Fks_DirEnt const* ent, char const* dirPath) FKS_NOEXCEPT {
	++((size_t*)pCnt)[0];
	((size_t*)pCnt)[1] += strlen(dirPath) + 1 + strlen(ent->name) + 1;
	return 1;
}

FKS_LIB_DECL (fks_isize_t)
fks_countDirEntries(Fks_DirEntries* dirEntries, int flags, Fks_DirEnt_IsMatchCB isMatch, size_t* strBytes ) FKS_NOEXCEPT
{
	size_t cnt[2] = {0};
	fks_foreachDirEntries(dirEntries, (strBytes) ? fks_countDirEntries_sub2 : fks_countDirEntries_sub, cnt, flags, isMatch);
	if (strBytes)
		*strBytes = cnt[1];
	return (fks_isize_t)cnt[0];
}


FKS_STATIC_DECL (fks_isize_t) fks_createDirEntPathStatSub(void** ppAry, char const* dirPath, char const* fname, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT;
FKS_STATIC_DECL (fks_isize_t) fks_convDirEntPathStatSub(void** ppAry, Fks_DirEntries* dirEntries, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT;

FKS_LIB_DECL (fks_isize_t)
fks_convDirEntPathStats(Fks_DirEntPathStat** ppPathStats , Fks_DirEntries* dirEntries, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	return fks_convDirEntPathStatSub((void**)ppPathStats , dirEntries, FKS_DE_NameStat|flags, isMatch);
}

FKS_LIB_DECL (fks_isize_t)
fks_convDirEntPaths(char*** pppPaths, Fks_DirEntries* dirEntries, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	return fks_convDirEntPathStatSub((void**)pppPaths, dirEntries, FKS_DE_NameStat|flags, isMatch);
}

FKS_LIB_DECL (fks_isize_t)
fks_createDirEntPathStats(Fks_DirEntPathStat** ppPathStats , char const* dirPath, char const* fname, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	return fks_createDirEntPathStatSub((void**)ppPathStats , dirPath, fname, FKS_DE_NameStat|flags, isMatch);
}

FKS_LIB_DECL (fks_isize_t)
fks_createDirEntPaths(char*** pppPaths, char const* dirPath,  char const* fname, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	return fks_createDirEntPathStatSub((void**)pppPaths, dirPath, fname, FKS_DE_Tiny|flags, isMatch);
}

FKS_STATIC_DECL (fks_isize_t)
fks_createDirEntPathStatSub(void** ppAry, char const* dirPath,  char const* fname, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	fks_isize_t		n;
	Fks_DirEntries	dirEntries;
	FKS_ARG_PTR_ASSERT(1, ppAry);
	if (!ppAry)
		return -1;
	*ppAry = NULL;
	if (fks_createDirEntries(&dirEntries, dirPath, fname, (flags & ~FKS_DE_FileOnly), isMatch) == NULL)
		return -1;
	if (dirEntries.size <= 0)
		return dirEntries.size;
	n = fks_convDirEntPathStatSub(ppAry, &dirEntries, flags, isMatch);
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
	char**				name;
	Fks_DirEntPathStat*	nameStat;
} Fks_DirEntNameStat_Uni;
typedef struct Fks_DirEntNameStat_Cur {
	Fks_DirEntNameStat_Uni	a;
	fks_stat_t*				stat;
	char* 					strs;
} Fks_DirEntNameStat_Cur;

static int fks_getDirEntNameStat_sub1(void* cur0, Fks_DirEnt const* d, char const* dirPath) FKS_NOEXCEPT;

FKS_STATIC_DECL (fks_isize_t)
fks_convDirEntPathStatSub(void** ppAry, Fks_DirEntries* dirEntries, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	fks_isize_t				n;
	fks_isize_t				cnt;
	size_t					strSz;
	size_t					nmSz;
	size_t					statSz;
	char*					buf;
	Fks_DirEntNameStat_Cur	cur;
	FKS_ARG_PTR_ASSERT(1, ppAry);
	if (!ppAry)
		return -1;
	*ppAry = NULL;
	if (dirEntries->size <= 0)
		return dirEntries->size;
	n 		= fks_countDirEntries(dirEntries, flags, isMatch, &strSz);
	if (n <= 0)
		return n;
	if (flags & FKS_DE_NameStat) {
		nmSz   = ((size_t)n + 1) * sizeof(Fks_DirEntPathStat);
		statSz = (size_t)n * sizeof(fks_stat_t);
	} else {
		nmSz   = ((size_t)n + 1) * sizeof(char*);
		statSz = 0;
		cur.stat = 0;
	}
	buf   = (char*)fks_calloc(1, nmSz + statSz + strSz);
	if (!buf)
		return -1;
	if (flags & FKS_DE_NameStat) {
		cur.a.nameStat  = (Fks_DirEntPathStat*)buf;
		cur.stat    	= (fks_stat_t*)(buf + nmSz);
		cur.strs		= buf + nmSz + statSz;
	} else {
		cur.a.name  	= (char**)buf;
		cur.stat    	= NULL;
		cur.strs		= buf + nmSz;
	}
	cnt = fks_foreachDirEntries(dirEntries, fks_getDirEntNameStat_sub1, &cur, flags, isMatch);
	if (cnt < 0) {
		//if (flags & FKS_DE_ErrCont)
		//	*ppAry    = buf;
		//else
			fks_free(cur.a.name);
		return cnt;
	}
	*ppAry    = buf;
	//FKS_ASSERT(n == cnt);
	return n;
}

static int fks_getDirEntNameStat_sub1(void* cur0, Fks_DirEnt const* d, char const* dirPath) FKS_NOEXCEPT
{
	Fks_DirEntNameStat_Cur*	c = (Fks_DirEntNameStat_Cur*)cur0;
	size_t 					l = strlen(dirPath) + 1 + strlen(d->name) + 1;
	fks_pathJoin(c->strs, l, dirPath, d->name);
	if (c->stat) {
		c->a.nameStat->path	= c->strs;
		c->a.nameStat->stat	= c->stat;
		*c->stat			= *d->stat;
		++c->stat;
		++c->a.nameStat;
	} else {
		*c->a.name = c->strs;
		++c->a.name;
	}
	c->strs += l;
	return 1;
}
