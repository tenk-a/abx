/*
 *  @file   fks_dirent.h
 *  @brief  
 *	@author	Masashi Kitamura (tenka@6809.net)
 *	@license Boost Software Lisence Version 1.0
 */
#ifndef FKS_DIRENTRY_H_INCLUDED
#define FKS_DIRENTRY_H_INCLUDED

#include <fks/fks_io.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Fks_DirEnt {
	fks_stat_t				st;
	char const* 			name;
	struct Fks_DirEntries*	sub;
} Fks_DirEnt;

typedef struct Fks_DirEntries {
	Fks_DirEnt const*		entries;
	size_t					size;
	char const*				name;
} Fks_DirEntries;

typedef int (*Fks_DirEnt_IsMatchCB)(Fks_DirEnt const*) FKS_NOEXCEPT;
int 	fks_dirEnt_isMatchStartWithNonDot(Fks_DirEnt const* ent) FKS_NOEXCEPT;

typedef int (*Fks_ForeachDirEntCB)(void* data, Fks_DirEnt const* dirEnt, char const* dirName) FKS_NOEXCEPT;

enum {
	FKS_DE_Recursive    = 1,	// Recursive directory
	FKS_DE_DotAndDotDot = 2,	// Get "." ".."
	FKS_DE_DirOnly      = 4,	// Directory only
	FKS_DE_FileOnly     = 8,	// File only (If there is no recurrence or when using fks_foreachDirEntries)
	FKS_DE_Tiny         = 16,	// For linux (readdir(), not use stat())
};
FKS_LIB_DECL (Fks_DirEntries*)	fks_getDirEntries(Fks_DirEntries* dirEntries, char const* dir, int flags FKS_ARG_INI(0), Fks_DirEnt_IsMatchCB isMatch FKS_ARG_INI(NULL)) FKS_NOEXCEPT;
//FKS_LIB_DECL (Fks_DirEntries*)	fks_getRecursiveDirEntries(Fks_DirEntries* dirEntries, char const* dir, Fks_DirEnt_IsMatchCB isMatch FKS_ARG_INI(NULL)) FKS_NOEXCEPT;
FKS_LIB_DECL (void)				fks_freeDirEntries(Fks_DirEntries* dirEntries) FKS_NOEXCEPT;
FKS_LIB_DECL (int)				fks_foreachDirEntries(Fks_DirEntries* dirEntries
										, int (*cb)(void* data, Fks_DirEnt const* dirEnt, char const* dirName) FKS_NOEXCEPT	// , Fks_ForeachDirEntCB cb
										, void* data FKS_ARG_INI(NULL)
										, int flags FKS_ARG_INI(0), Fks_DirEnt_IsMatchCB isMatch FKS_ARG_INI(NULL)) FKS_NOEXCEPT;
FKS_LIB_DECL (size_t)			fks_countDirEntries(Fks_DirEntries* dirEntries, int flags FKS_ARG_INI(0), Fks_DirEnt_IsMatchCB isMatch FKS_ARG_INI(NULL)) FKS_NOEXCEPT;

FKS_LIB_DECL (char**)			fks_getDirEntNames(char const* dir, int flags FKS_ARG_INI(0), Fks_DirEnt_IsMatchCB isMatch FKS_ARG_INI(NULL)) FKS_NOEXCEPT;
//FKS_LIB_DECL (char**)			fks_getRecursiveDirEntNames(char const* dir, int flags FKS_ARG_INI(1), Fks_DirEnt_IsMatchCB isMatch FKS_ARG_INI(NULL)) FKS_NOEXCEPT;
FKS_LIB_DECL (void)				fks_freeDirEntNames(char** dirEntryNames, size_t n) FKS_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
template<class CB>
int fks_foreachDirEntries(Fks_DirEntries* dirEntries, CB& cb, int flags = 0, Fks_DirEnt_IsMatchCB isMatch = NULL) FKS_NOEXCEPT
{
	FKS_ARG_PTR_ASSERT(1, dirEntries);
	if (!dirEntries)
		return 0;
	size_t		n       = dirEntries->size;
	Fks_DirEnt*	entries = dirEntries->entries;
	for (size_t i = 0; i < n; ++i) {
		Fks_DirEnt* d = &entries[i];
		if (!(flags & FKS_DE_DotAndDotDot) && (!strcmp(d->name, ".") || !strcmp(d->name, "..")))
			continue;
		if ((flags & FKS_DE_DirOnly) && !FKS_S_ISDIR(d->st.st_mode))
			continue;
		if ((flags & FKS_DE_FileOnly) && FKS_S_ISDIR(d->st.st_mode))
			continue;
		if (isMatch && isMatch(d) == 0)
			continue;
		if (cb(d) == 0)
			return 0;	// foreach break
		if (d->sub) {
			if (fks_foreachDirEntries(d->sub, invoke, isMatch) == 0)
				return 0;	// foreach break
		}
	}
	return 1;	// foreach continue
}
#endif

#endif
