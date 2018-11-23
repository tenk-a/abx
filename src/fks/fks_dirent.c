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
		size_t		i;
		size_t		l;
		size_t 		m			= 0;
		size_t 		n  			= dirEntries->size;
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
	size_t			i, n;
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

FKS_LIB_DECL (int)
fks_foreachDirEntries(Fks_DirEntries* dirEntries, Fks_ForeachDirEntCB cb, void* data, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	size_t				i, n;
	Fks_DirEnt const*	entries;
	char const*			dirName;
	FKS_ARG_PTR_ASSERT(1, dirEntries);
	FKS_ARG_PTR_ASSERT(1, invoke);
	if (!dirEntries || !cb)
		return 0;
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
			return 0;	// foreach break
		if (d->sub) {
			if (fks_foreachDirEntries(d->sub, cb, data, flags, isMatch) == 0)
				return 0;	// foreach break
		}
	}
	return 1;	// foreach continue
}


static int fks_countDirEntries_sub(void* pCnt, Fks_DirEnt const* ent, char const* dirName) FKS_NOEXCEPT
{
	++*(size_t*)pCnt;
	return 1;
}
FKS_LIB_DECL (size_t)
fks_countDirEntries(Fks_DirEntries* dirEntries, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	size_t cnt = 0;
	fks_foreachDirEntries(dirEntries, fks_countDirEntries_sub, &cnt, flags, isMatch);
	return cnt;
}


static int fks_getDirEntNames_sub(void* ppNames0, Fks_DirEnt const* d, char const* dirName) FKS_NOEXCEPT
{
	char** ppNames 	= (char**)ppNames0;
	size_t l 		= strlen(dirName) + 1 + strlen(d->name) + 1;
	char*  path		= path = (char*)fks_alloca(l);
	fks_pathJoin(path, l, dirName, d->name);
	*ppNames = strdup(path);
	if (*ppNames == NULL)
		return 0;
	++*ppNames;
	return 1;
}

FKS_LIB_DECL (size_t)
fks_getDirEntNames(char*** pppNames, char const* dirName, int flags, Fks_DirEnt_IsMatchCB isMatch) FKS_NOEXCEPT
{
	size_t			n;
	char**			names;
	Fks_DirEntries	dirEntries;
	if (!pppNames)
		return 0;
	*pppNames = NULL;
	if (fks_getDirEntries(&dirEntries, dirName, FKS_DE_Tiny|(flags & ~FKS_DE_FileOnly), isMatch) == NULL)
		return 0;
	if (dirEntries.size == 0)
		return 0;
	n 		= fks_countDirEntries(&dirEntries, flags, isMatch);
	names   = (char**)fks_calloc(1, sizeof(char*) * (n + 1));
	if (!names)
		return 0;
	if (fks_foreachDirEntries(&dirEntries, fks_getDirEntNames_sub, names, flags, isMatch) == 0) {
		fks_freeDirEntNames(names, n);
		fks_freeDirEntries(&dirEntries);
		return 0;
	}
	names[n]  = NULL;
	*pppNames = names;
	fks_freeDirEntries(&dirEntries);
	return n;
}


FKS_LIB_DECL (void)
fks_freeDirEntNames(char** dirEntryNames, size_t n) FKS_NOEXCEPT
{
	size_t			i;
	for (i = 0; i < n; ++i)
		fks_free(dirEntryNames[i]);
	fks_free(dirEntryNames);
}