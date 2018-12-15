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
#include <sys/stat.h>

typedef struct LinkData {
    struct LinkData*    link;
    struct dirent		dien;
    fks_stat_t			st;
} LinkData;

FKS_STATIC_DECL (Fks_DirEntries*)
fks_getDirEntries1(Fks_DirEntries* dirEntries, char const* dirPath
	, char const* pattern, int flags
	, Fks_DirEnt_IsMatchCB isMatch, void* isMatchData) FKS_NOEXCEPT
{
    char				pathBuf[FKS_PATH_MAX];
    unsigned        	num;
    int             	dirNum;
    int             	flag;
    int					st_ex_mode;
    char*         		baseName;
    size_t          	baseNameSz;
	size_t				n;
	size_t				l;
	size_t				strSz, entSz, statSz;
	DIR*				dir;
	struct dirent*		de;
	Fks_DirEnt*			d;
	LinkData*			t;
	LinkData*	 		linkData1;
    LinkData            root;
	char*				buf;
	fks_stat_t*			statp;
	char*				strp;
	char*				strp_end;

    FKS_ARG_PTR_ASSERT(1, dirEntries);
    FKS_ARG_PTR_ASSERT(2, dirPath);
    FKS_ARG_PTR_ASSERT(3, pattern);
    if (!dirEntries)
        return NULL;

    if (!dirPath && !pattern) {
        FKS_ASSERT(dirPath || pattern);
        return NULL;
    }
    if (*dirPath == '\0')
    	dirPath = ".";
    if (pattern == NULL || pattern[0] == 0)
        pattern = "*";

//printf(">%s %s\n", dirPath, pattern);
	dir = opendir(dirPath);
	if (dir == NULL)
		return NULL;
	if ((flags & (FKS_DE_Dir|FKS_DE_File)) == 0)
		flags |= FKS_DE_File;
	memset(pathBuf, 0, sizeof pathBuf);
	fks_pathCpy(pathBuf, FKS_PATH_MAX, dirPath);
	fks_pathCat(pathBuf, FKS_PATH_MAX, "/");
	baseName = pathBuf + strlen(pathBuf);
//printf("\t%s %s\n", pathBuf, baseName);

	memset(&root, 0, sizeof root);
	t = &root;
	num = 0;
	flag = 0;
	n = 0;
	linkData1 = NULL;
	strSz = strlen(dirPath) + 1;
	while ((de = readdir(dir)) != NULL) {
	    int				dirFlg;
		unsigned int	st_ex_mode = 0;
		if (FKS_DE_IsDotOrDotDot(de->d_name)) {
			st_ex_mode |= FKS_S_EX_DOTORDOTDOT;
			if (!(flags & FKS_DE_DotOrDotDot))
				continue;
		}
		if (!(flags & FKS_DE_Hidden) && de->d_name[0] == '.') {
//printf("\thidden skip\t%s\n", de->d_name);
			continue;
		}
		dirFlg = (de->d_type == DT_DIR);
		*baseName = 0;
		if (de->d_type == DT_UNKNOWN) {
			if (linkData1 == NULL)
				linkData1 = (LinkData*)fks_calloc(1, sizeof(LinkData));
			if (linkData1 == NULL)
				return NULL;
			fks_pathCpy(baseName, FKS_PATH_MAX, de->d_name);
			if (fks_lstat(pathBuf, &linkData1->st) < 0) {
				st_ex_mode |= FKS_S_EX_ERROR;
			}
			dirFlg = (linkData1->st.st_mode & S_IFDIR) != 0;
		}
//printf("\tdirFlg=%d\n", dirFlg);
		if (FKS_DE_IsDirOnly(flags)  && (de->d_type != DT_DIR) && (de->d_type != DT_UNKNOWN)) {
//printf("\tskip\t%s\n", de->d_name);
			continue;
		}
		if (FKS_DE_IsFileOnly(flags) && dirFlg) {
			if (!(flags & FKS_DE_Recursive)) {
//printf("\tskip2\t%s\n", de->d_name);
				continue;
			}
			st_ex_mode |= FKS_S_EX_NOTMATCH;
		}

		if (fks_pathMatchSpec(de->d_name, pattern) == 0) {
			//printf("pathMatchSpec %s %s %d\n", de->d_name, pattern, fks_pathMatchSpec(de->d_name, pattern));
			if (!(flags & FKS_DE_Recursive) || !dirFlg) {
				//printf("\tskip3\t%s\n", de->d_name);
				continue;
			} else {
				st_ex_mode |= FKS_S_EX_NOTMATCH;
			}
//printf("pathMatchSpec %s %s %d\n", de->d_name, pattern, fks_pathMatchSpec(de->d_name, pattern));
		}
		if (linkData1 == NULL)
			linkData1 = (LinkData*)fks_calloc(1, sizeof(LinkData));
		if (linkData1 == NULL)
			return NULL;
        strSz  += strlen(de->d_name) + 1;
//printf("linkData1 = %p strSz=%lld\n",linkData1, (long long)strSz);
		t->link = linkData1;
		linkData1->dien = *de;
		if ((flags & FKS_DE_Tiny) && de->d_type != DT_UNKNOWN) {
			linkData1->st.st_mode	= (dirFlg) ? S_IFDIR : 0;
			linkData1->st.st_ino	= de->d_ino;
//printf("st.st_mode=%02x ino=%d\n", linkData1->st.st_mode, linkData1->st.st_ino);
		} else
	 	if (!*baseName) {
			fks_pathCpy(baseName, FKS_PATH_MAX, de->d_name);
			if (fks_lstat(pathBuf, &linkData1->st) < 0) {
				st_ex_mode |= FKS_S_EX_ERROR;
			}
//printf("lstat %s\t%lld linkData1->st.st_mode=%2x\n",de->d_name, (long long)linkData1->st.st_size, linkData1->st.st_mode);
		}
		linkData1->st.st_d_type  = de->d_type;
		linkData1->st.st_ex_mode = st_ex_mode;
		if (isMatch) {
			Fks_DirEnt deWk = { de->d_name, &linkData1->st, NULL };
			if (!isMatch(isMatchData, &deWk)) {
				memset(linkData1, 0, sizeof *linkData1);	//free(linkData1); linkData1 = NULL;
				continue;
			}
		}
		t = linkData1;
		linkData1 = NULL;
		++n;
	}

	if (linkData1)
		fks_free(linkData1);
//printf("n=%lld\n",(long long)n);

    entSz  = (n + 1) * sizeof(Fks_DirEnt);
    statSz = n * sizeof(fks_stat_t);
//printf("entSz=%lld statSz=%lld strSz=%lld\n", (long long)entSz, (long long)statSz, (long long)strSz);
    buf    = (char*)fks_calloc(1, entSz + statSz + strSz + 1);
//printf("buf = %p entSz=%lld statSz=%lld strSz=%lld\n", buf, (long long)entSz, (long long)statSz, (long long)strSz);
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
//printf("%-23s\t%10lld\n",d->name, d->stat->st_size);
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
