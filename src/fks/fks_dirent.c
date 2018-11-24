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
fks_getDirEntries(Fks_DirEntries* dirEntries, char const* dirName, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	dirEntries = fks_getDirEntries1(dirEntries, dirName, flags, isMatch);
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
			if (FKS_S_ISDIR(d->st.st_mode)) {
				if (flags & FKS_DE_FileOnly)
					continue;
				Fks_DirEntries* des = (Fks_DirEntries*)fks_calloc(1, sizeof(Fks_DirEntries));
				if (!des)
					return NULL;
				l = strlen(dirName) + 1 + strlen(d->name) + 1;
				if (l < FKS_PATH_MAX)
					l = FKS_PATH_MAX;
				if (m < l) {
					m    = l;
					path = (char*)fks_realloc(path, l);
				}
				fks_pathJoin(path, l, dirName, d->name);
				if (fks_getDirEntries(des, path, flags, isMatch)) {
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
fks_freeDirEntries(Fks_DirEntries* dirEntries) FKS_NOEXCEPT
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
			fks_freeDirEntries(d->sub);
	}
	fks_free(entries);
	dirEntries->entries	= NULL;
	dirEntries->name	= NULL;
	dirEntries->size	= 0;
}

FKS_LIB_DECL (fks_isize_t)
fks_foreachDirEntries(Fks_DirEntries* dirEntries, Fks_ForeachDirEntCB cb, void* data, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	fks_isize_t			i, n, cnt;
	Fks_DirEnt const*	entries;
	char const*			dirName;
	FKS_ARG_PTR_ASSERT(1, dirEntries);
	FKS_ARG_PTR_ASSERT(1, invoke);
	if (!dirEntries || !cb)
		return -1;
	cnt		= 0;
	n       = dirEntries->size;
	entries = dirEntries->entries;
	dirName = dirEntries->name;
	for (i = 0; i < n; ++i) {
		Fks_DirEnt const* d = &entries[i];
		if (!(flags & FKS_DE_DotAndDotDot) && (!strcmp(d->name, ".") || !strcmp(d->name, "..")))
			continue;
		if ((flags & FKS_DE_DirOnly) && !FKS_S_ISDIR(d->st.st_mode))
			continue;
		if ((flags & FKS_DE_FileOnly) && FKS_S_ISDIR(d->st.st_mode))
			continue;
		if (isMatch && isMatch(d) == 0)
			continue;
		if (cb(data, d, dirName) == 0)
			return -1 - cnt;	// foreach break
		++cnt;
		if (d->sub) {
			fks_isize_t cnt2 = fks_foreachDirEntries(d->sub, cb, data, flags, isMatch);
			if (cnt2 < 0)
				return -1 - cnt + cnt2;	// foreach break
			cnt += cnt2;
		}
	}
	return cnt;	// foreach continue
}


static int fks_countDirEntries_sub(void* pCnt, Fks_DirEnt const* dmy_ent, char const* dmy_dirName) FKS_NOEXCEPT {
	++*(size_t*)pCnt;
	return 1;
}
static int fks_countDirEntries_sub2(void* pCnt, Fks_DirEnt const* ent, char const* dirName) FKS_NOEXCEPT {
	++((size_t*)pCnt)[0];
	((size_t*)pCnt)[1] += strlen(dirName) + 1 + strlen(ent->name) + 1;
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


FKS_STATIC_DECL (fks_isize_t) fks_getDirEntNameStatSub(void** ppAry, char const* dirName, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT;

FKS_LIB_DECL (fks_isize_t)
fks_getDirEntNameStats(Fks_DirEntNameStat** ppNameStats, char const* dirName, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	return fks_getDirEntNameStatSub((void**)ppNameStats, dirName, FKS_DE_NameStat|flags, isMatch);
}

FKS_LIB_DECL (fks_isize_t)
fks_getDirEntNames(char*** pppNames, char const* dirName, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	return fks_getDirEntNameStatSub((void**)pppNames, dirName, FKS_DE_Tiny|flags, isMatch);
}

typedef union Fks_DirEntNameStat_Uni {
	char**				name;
	Fks_DirEntNameStat*	nameStat;
} Fks_DirEntNameStat_Uni;
typedef struct Fks_DirEntNameStat_Cur {
	Fks_DirEntNameStat_Uni	a;
	fks_stat_t*				stat;
	char* 					strs;
} Fks_DirEntNameStat_Cur;

static int fks_getDirEntNameStat_sub1(void* cur0, Fks_DirEnt const* d, char const* dirName) FKS_NOEXCEPT
{
	Fks_DirEntNameStat_Cur*	c = (Fks_DirEntNameStat_Cur*)cur0;
	size_t 					l = strlen(dirName) + 1 + strlen(d->name) + 1;
	fks_pathJoin(c->strs, l, dirName, d->name);
	if (c->stat) {
		c->a.nameStat->path		= c->strs;
		*c->a.nameStat->stat	= d->st;
		++c->stat;
		++c->a.nameStat;
	} else {
		*c->a.name = c->strs;
		++c->a.name;
	}
	c->strs += l;
	return 1;
}

FKS_STATIC_DECL (fks_isize_t)
fks_getDirEntNameStatSub(void** ppAry, char const* dirName, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	fks_isize_t				n;
	fks_isize_t				cnt;
	size_t					strSz;
	size_t					nmSz;
	size_t					statSz;
	char*					buf;
	Fks_DirEntNameStat_Cur	cur;
	Fks_DirEntries			dirEntries;
	FKS_ARG_PTR_ASSERT(1, ppAry);
	if (!ppAry)
		return -1;
	*ppAry = NULL;
	if (fks_getDirEntries(&dirEntries, dirName, (flags & ~FKS_DE_FileOnly), isMatch) == NULL)
		return -1;
	if (dirEntries.size <= 0)
		return dirEntries.size;
	n 		= fks_countDirEntries(&dirEntries, flags, isMatch, &strSz);
	if (n <= 0)
		return n;
	if (flags & FKS_DE_NameStat) {
		nmSz   = ((size_t)n + 1) * sizeof(Fks_DirEntNameStat);
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
		cur.a.nameStat  = (Fks_DirEntNameStat*)buf;
		cur.stat    	= (fks_stat_t*)(buf + nmSz);
		cur.strs		= buf + nmSz + statSz;
	} else {
		cur.a.name  	= (char**)buf;
		cur.stat    	= NULL;
		cur.strs		= buf + nmSz;
	}
	cnt = fks_foreachDirEntries(&dirEntries, fks_getDirEntNameStat_sub1, &cur, flags, isMatch);
	if (cnt <= 0) {
		fks_freeDirEntNames((char**)cur.a.name);
		fks_freeDirEntries(&dirEntries);
		return cnt;
	}
	*ppAry    = buf;
	fks_freeDirEntries(&dirEntries);
	FKS_ASSERT(n == cnt);
	return n;
}


FKS_LIB_DECL (void)
fks_freeDirEntNameStats(Fks_DirEntNameStat** nameStats) FKS_NOEXCEPT
{
	fks_free(nameStats);
}


FKS_LIB_DECL (void)
fks_freeDirEntNames(char** names) FKS_NOEXCEPT
{
	fks_free(names);
}
