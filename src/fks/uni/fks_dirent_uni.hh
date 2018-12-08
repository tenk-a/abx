/*
 *  @file   fks_dirent_uni.hh
 *  @brief
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks_dirent.h>
#include <fks_malloc.h>
#include <fks_assert_ex.h>
#include <fks_path.h>
#include <fks_io.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <fnmatch.h>

typedef struct LinkData {
    struct LinkData*    link;
    struct dirent		dien;
    fks_stat_t			st;
} LinkData;

FKS_LIB_DECL (Fks_DirEntries*)
fks_getDirEntries1(Fks_DirEntries* dirEntries, char const* dirPath
	, char const* pattern, int flags
	, Fks_DirEnt_IsMatchCB isMatch, void* isMatchData) FKS_NOEXCEPT
{
    char				pathBuf[FKS_PATH_MAX] = {0};
    unsigned        	num      = 0;
    int             	dirNum;
    //char*				srchBase = fks_pathBaseName(dirPath);
    char*         		baseName;
    size_t          	baseNameSz;
    int             	flag = 0;
    int					st_ex_mode;
    LinkData            root = { 0 };
	DIR*				dir;
	LinkData*			t;
	size_t				n;
	size_t				l;
	size_t				strSz, entSz, statSz;
	struct dirent*		de;
	char*				buf;
	Fks_DirEnt*			d;
	fks_stat_t*			statp;
	char*				strp;
	char*				strp_end;

	dir = opendir(dirPath);
	if (dir == NULL)
		return NULL;
	if (flags & FKS_DE_Recursive)
		flags |= FKS_DE_Dir;

	fks_pathCpy(pathBuf, FKS_PATH_MAX, dirPath);
	fks_pathCat(pathBuf, FKS_PATH_MAX, "/");
	baseName = pathBuf + strlen(pathBuf);

	t = &root;
	n = 0;
	strSz = 0;
	while ((de = readdir(dir)) != NULL) {
		LinkData*	 u;
		unsigned int st_ex_mode = 0;
		if (FKS_DE_IsDotOrDotDot(de->d_name)) {
			st_ex_mode |= FKS_S_EX_DOTORDOTDOT;
			if (flags & FKS_DE_DotOrDotDot)
				continue;
		}
	 #if defined _DIRENT_HAVE_D_TYPE && defined _BSD_SOURCE
		if (FKS_DE_IsDirOnly(flags)  && (de->d_type != DT_DIR) && (de->d_type != DT_UNKOWN))
			continue;
		if (FKS_DE_IsFileOnly(flags) && (de->d_type == DT_DIR) && (de->d_type != DT_UNKOWN))
			continue;
	 #endif
		if (fnmatch(de->d_name, pattern, 0) == 0)
			continue;
		u = fks_calloc(1, sizeof(LinkData));
		if (u == NULL)
			return NULL;
        strSz  += strlen(de->d_name) + 1;
		t->link = u;
		u->dien = *de;
	 #if defined _DIRENT_HAVE_D_TYPE && defined _BSD_SOURCE
		if ((flags & FKS_DE_Tiny) && de->d_type != DT_UNKOWN) {
			u->st.st_mode	= ((de->d_type == DT_DIR) ? S_DIR : 0);
			u->st.st_ino	= de->d_ino;
		} else
	 #endif
	 	{
			fks_stat_t st;
			fks_pathCpy(baseName, FKS_PATH_MAX, de->d_name);
			if (fks_lstat(pathBuf, &st) < 0)
				st_ex_mode |= FKS_S_EX_ERROR;
		}
		u->st.st_d_type  = de->d_type;
		u->st.st_ex_mode = st_ex_mode;
		if (isMatch) {
			Fks_DirEnt deWk = { de->d_name, &u->st, NULL };
			if (!isMatch(isMatchData, &deWk)) {
				free(u);
				continue;
			}
		}
		t = u;
		++n;
	}

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
        *statp  = t->st;
        d->stat = statp++;
        l = strlen(t->dien.d_name) + 1;
        memcpy(strp, t->dien.d_name, l);
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
