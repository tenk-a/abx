/*
 *  @file   fks_dirent.h
 *  @brief
 *	@author	Masashi Kitamura (tenka@6809.net)
 *	@license Boost Software Lisence Version 1.0
 */
#ifndef FKS_DIRENTRY_H_INCLUDED
#define FKS_DIRENTRY_H_INCLUDED

#include <fks/fks_io.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Fks_DirEnt {
	char const* 			name;
	fks_stat_t*				stat;
	struct Fks_DirEntries*	sub;
} Fks_DirEnt;

typedef struct Fks_DirEntries {
	Fks_DirEnt const*		entries;
	fks_isize_t				size;		// >=0:entries count  <0: has error.
	char const*				path;
} Fks_DirEntries;

typedef int (*Fks_DirEnt_IsMatchCB)(Fks_DirEnt const*) FKS_NOEXCEPT;
int 	fks_dirEnt_isMatchStartWithNonDot(Fks_DirEnt const* ent) FKS_NOEXCEPT;

typedef int (*Fks_ForeachDirEntCB)(void* data, Fks_DirEnt const* dirEnt, char const* dirPath) FKS_NOEXCEPT;

enum {
	FKS_DE_Recursive    = 1,	// Recursive directory
	FKS_DE_DotAndDotDot = 2,	// Get "." ".."
	FKS_DE_DirOnly      = 4,	// Directory only
	FKS_DE_FileOnly     = 8,	// File only (If there is no recurrence or when using fks_foreachDirEntries)
	//FKS_DE_ErrCont	= 0x80,	// Continue even if an error occurs. Do not delete the created part.

	FKS_DE_Tiny         = 0x1000000,	// For linux (readdir(), not use stat())
	FKS_DE_NameStat		= 0x2000000,	// fks_createDirEntPaths: char** -> Fks_DE_NameStat*    (fks_countDirEntries)
};
FKS_LIB_DECL (Fks_DirEntries*)	fks_createDirEntries(Fks_DirEntries* dirEntries, char const* dirPath, char const* fname FKS_ARG_INI(NULL)
										, unsigned int flags FKS_ARG_INI(0), Fks_DirEnt_IsMatchCB isMatch FKS_ARG_INI(NULL)) FKS_NOEXCEPT;
FKS_LIB_DECL (void)				fks_releaseDirEntries(Fks_DirEntries* dirEntries) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_isize_t)		fks_foreachDirEntries(Fks_DirEntries* dirEntries
										, int (*cb)(void* data, Fks_DirEnt const* dirEnt, char const* dirPath) FKS_NOEXCEPT	// , Fks_ForeachDirEntCB cb
										, void* data FKS_ARG_INI(NULL), unsigned int flags FKS_ARG_INI(0)
										, Fks_DirEnt_IsMatchCB isMatch FKS_ARG_INI(NULL)) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_isize_t)		fks_countDirEntries(Fks_DirEntries* dirEntries, unsigned int flags FKS_ARG_INI(0)
										, Fks_DirEnt_IsMatchCB isMatch FKS_ARG_INI(NULL), size_t* strBytes FKS_ARG_INI(NULL)) FKS_NOEXCEPT;


typedef struct Fks_DirEntPathStat {
	char const* 	path;
	fks_stat_t*		stat;
} Fks_DirEntPathStat;

FKS_LIB_DECL (fks_isize_t)		fks_convDirEntPathStats(Fks_DirEntPathStat** ppPathStats , Fks_DirEntries* dirEntries
										, unsigned int flags FKS_ARG_INI(0), Fks_DirEnt_IsMatchCB isMatch FKS_ARG_INI(NULL)) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_isize_t)		fks_createDirEntPathStats(Fks_DirEntPathStat** ppPathStats , char const* dirPath, char const* fname FKS_ARG_INI(NULL)
										, unsigned int flags FKS_ARG_INI(0), Fks_DirEnt_IsMatchCB isMatch FKS_ARG_INI(NULL)) FKS_NOEXCEPT;
FKS_LIB_DECL (void)				fks_releaseDirEntPathStats(Fks_DirEntPathStat* pathStats) FKS_NOEXCEPT;

FKS_LIB_DECL (fks_isize_t)		fks_convDirEntPaths(char*** ppPaths, Fks_DirEntries* dirEntries
										, unsigned int flags FKS_ARG_INI(0), Fks_DirEnt_IsMatchCB isMatch FKS_ARG_INI(NULL)) FKS_NOEXCEPT;
FKS_LIB_DECL (fks_isize_t)		fks_createDirEntPaths(char*** ppPaths, char const* dirPath, char const* fname FKS_ARG_INI(NULL)
										, unsigned int flags FKS_ARG_INI(0), Fks_DirEnt_IsMatchCB isMatch FKS_ARG_INI(NULL)) FKS_NOEXCEPT;
FKS_LIB_DECL (void)				fks_releaseDirEntPaths(char** paths) FKS_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
template<class CB>
fks_isize_t fks_foreachDirEntries(Fks_DirEntries* dirEntries, CB& cb, unsigned int flags = 0, Fks_DirEnt_IsMatchCB isMatch = NULL) FKS_NOEXCEPT
{
 #ifdef FKS_ARG_PTR_ASSERT
	FKS_ARG_PTR_ASSERT(1, dirEntries);
 #endif
	if (!dirEntries)
		return -2;
	fks_isize_t 		cnt		= 0;
	fks_isize_t			n       = dirEntries->size;
	Fks_DirEnt const*	entries = dirEntries->entries;
	char const*			dirPath = dirEntries->path;
	for (fks_isize_t i = 0; i < n; ++i) {
		Fks_DirEnt const* d = &entries[i];
		if (!(flags & FKS_DE_DotAndDotDot) && (!strcmp(d->name, ".") || !strcmp(d->name, "..")))
			continue;
		if ((flags & FKS_DE_DirOnly) && !FKS_S_ISDIR(d->stat->st_mode))
			continue;
		if ((flags & FKS_DE_FileOnly) && FKS_S_ISDIR(d->stat->st_mode))
			continue;
		if (isMatch && isMatch(d) == 0)
			continue;
		if (cb(d, dirPath) == 0)
			return -1;	// foreach break
		++cnt;
		if (d->sub) {
			fks_isize_t cnt2 = fks_foreachDirEntries(d->sub, cb, flags, isMatch);
			if (cnt2 < 0)
				return cnt2;	// foreach break
			cnt += cnt2;
		}
	}
	return cnt;	// foreach continue
}
#endif

#endif
